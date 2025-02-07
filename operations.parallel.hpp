#pragma once

#include "opcode.hpp"
#include <algorithm>
#include <chrono>

#ifndef MIZU_NO_HARDWARE_THREADS
	#include <thread>
#endif

namespace mizu {
	// NOTE: Not a valid operation
	inline uint64_t new_thread(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
#ifndef MIZU_NO_HARDWARE_THREADS
		registers_and_stack env;
		std::copy(registers, registers + memory_size, env.memory.data());

		return (uint64_t)new std::thread([pc, env = std::move(env)]() mutable {
			setup_enviornment(env);
			return pc->op(pc, env.memory.data(), env.stack_boundary, env.stack_pointer);
		});
#else // MIZU_NO_HARDWARE_THREADS
		auto env = (registers_and_stack*)malloc(sizeof(registers_and_stack));
		std::copy(registers, registers + memory_size, env->memory.data());
		setup_enviornment(*env);
		mizu::coroutine::start(pc, env);
		return fpda_size(mizu::coroutine::contexts) - 1; // Return the index of the thread in the context
#endif // MIZU_NO_HARDWARE_THREADS
	}

#ifndef MIZU_CUSTOM_DELAY_IMPLEMENTATION
	inline void delay(std::chrono::microseconds time, uint64_t& storage_register) {
	#ifndef MIZU_NO_HARDWARE_THREADS
		std::this_thread::sleep_for(time);
	#else
		if(!storage_register) storage_register = (size_t)new std::chrono::high_resolution_clock::time_point(std::chrono::high_resolution_clock::now());

		auto start = (std::chrono::high_resolution_clock::time_point*)storage_register;
		if(std::chrono::high_resolution_clock::now() - *start > time)
			--mizu::coroutine::get_current_context().program_counter;
		else delete start;
	#endif
	}
#else 
	extern void delay(std::chrono::microseconds time);
#endif

	inline namespace operations { extern "C" {

		// Interpret register as signed
		void* fork_relative(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto threadPC = pc + *(int64_t*)&registers[pc->a];
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack, sp);
			MIZU_NEXT();
		}
#else
		;
#endif

		// Interpret arguments as signed
		void* fork_relative_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto threadPC = pc + *(int32_t*)&pc->a;
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack, sp);
			MIZU_NEXT();
		}
#else
		;
#endif

		void* fork_to(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto threadPC = (opcode*)registers[pc->a];
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack, sp);
			MIZU_NEXT();
		}
#else
		;
#endif

		void* join_thread(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
	#ifndef MIZU_NO_HARDWARE_THREADS
			std::thread* thread = (std::thread*)registers[pc->a];
			if(thread) {
				if(thread->joinable()) thread->join();
				delete thread;
				registers[pc->a] = 0;
			}
	#else // MIZU_NO_HARDWARE_THREADS
			size_t context_id = (size_t&)registers[pc->a];
			if(context_id) // Can't join the main thread!
				if(!mizu::coroutine::get_context(context_id).done()) {
					// If the thread in question is not done yet, back up the program counter (aka queue this same operation up to be run next)
					--mizu::coroutine::get_current_context().program_counter;
				} else registers[pc->a] = 0;
	#endif // MIZU_NO_HARDWARE_THREADS
			MIZU_NEXT();
		}
#else
		;
#endif

		void* delay_microseconds(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto time = std::chrono::microseconds(registers[pc->a]);
			delay(time, registers[pc->out]);
			MIZU_NEXT();
		}
#else
		;
#endif

		void* delay_seconds_f64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto seconds = (double&)registers[pc->a];
			size_t micro = seconds * 1'000'000;
			delay(std::chrono::microseconds{micro});
			MIZU_NEXT();
		}
#else
		;
#endif

		void* delay_seconds_f32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto seconds = (float&)registers[pc->a];
			size_t micro = seconds * 1'000'000;
			delay(std::chrono::microseconds{micro});
			MIZU_NEXT();
		}
#else
		;
#endif
	}}
}