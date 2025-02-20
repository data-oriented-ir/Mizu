#pragma once
#ifdef MIZU_IMPLEMENTATION
#define FP_IMPLEMENTATION
#endif
#include <fp/pointer.hpp>
#include <fp/dynarray.hpp>

#ifndef MIZU_REGISTER_OPERATION
#define MIZU_REGISTER_OPERATION(name)
#endif

#ifdef _WIN32
    #define MIZU_EXPORT __declspec(dllexport)
#elif __EMSCRIPTEN__
	#include <emscripten.h>
	#define MIZU_EXPORT EMSCRIPTEN_KEEPALIVE
#else
    #define MIZU_EXPORT __attribute__((__visibility__("default")))
#endif
#define MIZU_EXPORT_C extern "C" MIZU_EXPORT

#if defined(__clang__) || defined(MIZU_FORCE_ENABLE_TAIL_CALL)
	#define MIZU_TAIL_CALL __attribute__((musttail)) 
#else
	#define MIZU_TAIL_CALL 
#endif

#ifdef _WIN32
	#define MIZU_MAIN(...) MIZU_EXPORT_C int dll_main(const int argc, const char** argv __VA_ARGS__)
#else
	#define MIZU_MAIN(...) int main(const int argc, const char** argv __VA_ARGS__)
#endif



namespace mizu {
	using reg_t = uint16_t;
	inline namespace registers {
		constexpr reg_t x(std::size_t i) { return i; }
		constexpr reg_t t(std::size_t i) { return i + 1; assert(i <= 10); }
		constexpr reg_t a(std::size_t i) { return i + 12; }

		constexpr static reg_t zero = x(0);
		constexpr static reg_t return_address = x(11), ra = return_address;
	}
	using operation_t = void*(*)(struct opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);

	struct opcode {
		operation_t op;
		reg_t out, a, b;

		opcode& set_immediate(const uint32_t value) { (uint32_t&)a = value; return *this; }
		opcode& set_immediate_signed(const int32_t value) { (int32_t&)a = value; return *this; }
		opcode& set_branch_immediate(const int16_t value) { (int16_t&)b = value; return *this; }

		opcode& set_immediate_f32(const float value) { (float&)a = value; return *this; }
		opcode& set_lower_immediate_f64(const double value) { return set_immediate((uint64_t&)value); }
		opcode& set_upper_immediate_f64(const double value) { return set_immediate(((uint64_t&)value) >> 32); }

		opcode& set_host_pointer_lower_immediate(const void* ptr) { set_immediate((std::size_t)ptr); return *this; }
		opcode& set_host_pointer_upper_immediate(const void* ptr) { set_immediate(((std::size_t)ptr) >> 32); return *this; }
	};

	constexpr static reg_t memory_size = 1024 * 5;
	struct registers_and_stack {
		fp::array<uint64_t, memory_size> memory;
		uint8_t* stack_boundary;
		uint8_t* stack_pointer;
	};
	inline void setup_enviornment(registers_and_stack& env) {
		env.memory[0] = 0;
		env.stack_boundary = (uint8_t*)(env.memory.data() + 256);
		env.stack_pointer = (uint8_t*)(env.memory.data() + env.memory.size());
	}

#ifndef MIZU_NO_HARDWARE_THREADS
	#define MIZU_NEXT()  registers[0] = 0; MIZU_TAIL_CALL return (++pc)->op(pc, registers, stack_boundary, sp)

	#define MIZU_START_FROM_ENVIORNMENT(program, env) const_cast<opcode*>(program)->op(const_cast<opcode*>(program), env.memory.data(), env.stack_boundary, env.stack_pointer)
#else // MIZU_NO_HARDWARE_THREADS
	struct coroutine {
		struct execution_context {
			opcode* program_counter; // Set to null to indicate that a section is done
			uint8_t* stack_boundary_pointer;
			registers_and_stack* enviornment;

			inline bool done() {
				return !program_counter; // Done when program counter is null
			}
		};

		static fp_dynarray(execution_context) contexts;
		static size_t current_context /* = 0 */;

		inline static size_t next_context() {
			return current_context = (current_context + 1) % fpda_size(contexts);
		}

		inline static execution_context& get_context(size_t index) {
			assert(index >= 0);
			assert(index < fpda_size(contexts));
			return contexts[index];
		}

		inline static execution_context& get_current_context() {
			return get_context(current_context);
		}

		inline static void* next(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) { // NOTE: Next has the same signature as operations for tail recursion purposes!
			assert(sp);
			get_current_context().stack_pointer = sp;

			if(fpda_size(contexts) == 1) {
				get_current_context().program_counter = ++pc;
				MIZU_TAIL_CALL return pc->op(pc, registers, stack, sp);
			} else get_current_context().program_counter = pc; // Make sure pc updates (jumps) are recorded

			auto& context = get_context(next_context());
			if(!context.program_counter) return next(pc, registers, stack, context.stack_pointer); // If this context is done... recursively run the next one
			pc = ++context.program_counter;
			MIZU_TAIL_CALL return pc->op(pc, context.enviornment->memory.data(), context.enviornment->stack_boundary, context.stack_pointer);
		}

		static void start(opcode* program_counter, registers_and_stack* enviornment) {
			assert(program_counter && enviornment);
			execution_context ctx{program_counter - 1, enviornment->stack_pointer, enviornment};
			fpda_push_back(contexts, ctx);
		}

		static bool done() { // TODO: There has to be a better way to write this!
			fp_iterate_named(contexts, context)
				if(!context->done()) // We are done if all program counters are nullptr
					return false;
			return true;
		}
	};

	#define MIZU_NEXT() registers[0] = 0; MIZU_TAIL_CALL return mizu::coroutine::next(pc, registers, stack_boundary, sp)

	#define MIZU_START_FROM_ENVIORNMENT(program, env) [](mizu::opcode* program_counter, mizu::registers_and_stack* enviornment) -> void* {\
		mizu::coroutine::start(program_counter, enviornment);\
		void* result;\
		while(!(result = mizu::coroutine::next(\
			mizu::coroutine::get_current_context().program_counter, mizu::coroutine::get_current_context().enviornment->memory.data(),\
			mizu::coroutine::get_current_context().enviornment->stack_boundary, mizu::coroutine::get_current_context().stack_pointer)\
		) && !mizu::coroutine::done());\
		return result;\
	}(const_cast<mizu::opcode*>(program), &env)

#ifdef MIZU_IMPLEMENTATION
	fp_dynarray(coroutine::execution_context) coroutine::contexts = nullptr;
	size_t coroutine::current_context = 0;
#endif
#endif // MIZU_NO_HARDWARE_THREADS
}