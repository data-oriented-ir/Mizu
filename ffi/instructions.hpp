#pragma once

#include "../mizu/opcode.hpp"

#ifndef MIZU_NO_FFI

	#ifdef MIZU_IMPLEMENTATION
		#include "dynamic_loader.hpp"

		#ifndef MIZU_NO_LIB_FFI
			#include <ffi.h>
		#else
			#include "wasm/gen/calls.hpp"
		#endif
	#endif


	namespace mizu {
		namespace ffi {
	#ifdef MIZU_IMPLEMENTATION
		#ifndef MIZU_NO_LIB_FFI
			static thread_local fp::raii::dynarray<ffi_type*> current_types = {};
		#else
			static thread_local fp::raii::dynarray<wasm::types> current_types = {};
		#endif
	#endif


		template<bool has_return>
		void* call_impl(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
		{
	#ifndef MIZU_NO_LIB_FFI
			auto func = FFI_FN(registers[pc->a]);
			auto interface_ = (ffi_cif*)registers[pc->b];

			{
				fp::raii::dynarray<void*> args; args.reserve(interface_->nargs);
				for(size_t i = 0; i < interface_->nargs; ++i)
					args.push_back(&registers[registers::a(i)]);

				if constexpr(has_return)
					ffi_call(interface_, func, &registers[pc->out], args.data());
				else ffi_call(interface_, func, nullptr, args.data());
			}
	#else
			auto func = (void*)registers[pc->a];
			auto interface_ = (fp::dynarray<wasm::types>*)registers[pc->b];

			wasm::variant res;
			switch(interface_->size()) {
				break; case 0: MIZU_THROW(std::runtime_error("Function interfaces must at least specify the return type!"));
				break; case 1: res = wasm::call((*interface)[0], func);
				break; case 2: res = wasm::call((*interface)[0], func, (*interface)[1], registers[registers::a(0)]);
				break; case 3: res = wasm::call((*interface)[0], func, (*interface)[1], registers[registers::a(0)], (*interface)[2], registers[registers::a(1)]);
				break; case 4: res = wasm::call((*interface)[0], func, (*interface)[1], registers[registers::a(0)], (*interface)[2], registers[registers::a(1)],
					(*interface)[3], registers[registers::a(2)]);
				break; case 5: res = wasm::call((*interface)[0], func, (*interface)[1], registers[registers::a(0)], (*interface)[2], registers[registers::a(1)],
					(*interface)[3], registers[registers::a(2)], (*interface)[4], registers[registers::a(3)]);
				break; default: MIZU_THROW(std::runtime_error("The WASM Trampoline currently only supports functions with a maximum of 4 arguments!"));
			}

			auto& reg = registers[pc->out];
			if constexpr(has_return) std::visit([&reg](auto v) {
				if constexpr(!std::is_same_v<decltype(v), std::monostate>)
					reg = *(uint64_t*)&v;
			}, res);
	#endif
			MIZU_NEXT();
		}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION

		inline namespace instructions { extern "C" {

			/**
			 * Adds void to the current type stack.
			 */
			void* push_type_void(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_void);
	#else
				current_types.push_back(wasm::types::Void);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_void);

			/**
			 * Adds void* to the current type stack.
			 */
			void* push_type_pointer(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_pointer);
	#else
				if constexpr(sizeof(void*) == 4)
					current_types.push_back(wasm::types::I32);
				else if constexpr(sizeof(void*) == 8)
					current_types.push_back(wasm::types::I64);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_pointer);

			/**
			 * Adds int32_t to the current type stack.
			 */
			void* push_type_i32(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_sint32);
	#else
				current_types.push_back(wasm::types::I32);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_i32);

			/**
			 * Adds uint32_t to the current type stack.
			 */
			void* push_type_u32(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_uint32);
	#else
				current_types.push_back(wasm::types::I32);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_u32);

			/**
			 * Adds int64_t to the current type stack.
			 */
			void* push_type_i64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_sint64);
	#else
				current_types.push_back(wasm::types::I64);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_i64);

			/**
			 * Adds uint64_t to the current type stack.
			 */
			void* push_type_u64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_uint64);
	#else
				current_types.push_back(wasm::types::I64);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_u64);

			/**
			 * Adds `float` to the current type stack.
			 */
			void* push_type_f32(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_float);
	#else
				current_types.push_back(wasm::types::F32);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_f32);

			/**
			 * Adds `double` to the current type stack.
			 */
			void* push_type_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				current_types.push_back(&ffi_type_double);
	#else
				current_types.push_back(wasm::types::F64);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(push_type_f64);

			/**
			 * Clears the current type stack.
			 */
			void* clear_type_stack(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
				current_types.clear();
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(clear_type_stack);

			/**
			 * Converts the current type stack into a interface that tells the ffi subsystem what the types of each argument are.
			 * @note The first type on the stack is interpreted as the return type, any types after that are parameters (that one return type is required! Use push_type_void to indicate no return.)
			 *
			 * @param out Register to store the resulting interface in
			 */
			void* create_interface(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				auto interface_ = (ffi_cif*)(registers[pc->out] = (size_t)new ffi_cif{});

				ffi_status status = ffi_prep_cif(interface_, FFI_DEFAULT_ABI, current_types.size() - 1, current_types[0], current_types.data() + 1);
				if (status != FFI_OK) MIZU_THROW(std::runtime_error("ffi_prep_cif failed: " + std::to_string(status)));
	#else
				if(current_types.empty()) MIZU_THROW(std::runtime_error("Function interfaces must at least specify the return type!"));
				if(current_types.size() > 5) MIZU_THROW(std::runtime_error("The WASM Trampoline currently only supports functions with a maximum of 4 arguments!"));
				registers[pc->out] = (size_t)current_types.clone();
	#endif
				return clear_type_stack(pc, registers, env, sp);
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(create_interface);

			/**
			 * Frees the provided interface and overwrites it with the value in \p b (defaults to zero)
			 *
			 * @param a Register storing the interface to free.
			 * @param b Register storing the value to overwrite a with (defaults to zero)
			 */
			void* free_interface(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifndef MIZU_NO_LIB_FFI
				auto interface_ = (ffi_cif*)registers[pc->a];
				if(!interface_) MIZU_THROW(std::runtime_error("FFI Interface does not exist!"));
				fpda_free(interface_->arg_types - 1);
				delete interface_;
	#else
	
				auto interface_ = (wasm::types*)registers[pc->a];
				if(!interface_) MIZU_THROW(std::runtime_error("FFI Interface does not exist!"));
				fpda_free(interface_);
	#endif
				registers[pc->a] = registers[pc->b];
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(free_interface);

			/**
			 * Calls an ffi function, the functions arguments are expected to be in the argument registers (a0, a1, etc...)
			 * 
			 * @param a Register storing a pointer to the function to call.
			 * @param b Register storing the interface defining how the function's arguments should be interpreted.
			 */
			void* call(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
				return call_impl<false>(pc, registers, env, sp);
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(call);

			/**
			 * Calls an ffi function with a returned value, the functions arguments are expected to be in the argument registers (a0, a1, etc...)
			 * 
			 * @param out Register storing 
			 * @param a Register storing a pointer to the function to call.
			 * @param b Register storing the interface defining how the function's arguments should be interpreted.
			 */
			void* call_with_return(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
				return call_impl<true>(pc, registers, env, sp);
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(call_with_return);

			/**
			 * Loads a DLL (or platform equivalent) with the provided path.
			 * @note most platforms will search some set of library path for the given path
			 * 
			 * @param out Register to store the resulting library pointer in
			 * @param a Register storing a null-terminated c-string pointer representing the path to search for the library.
			 */
			void* load_library(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifdef MIZU_WASM
				MIZU_THROW(std::runtime_error("WASM doesn't currently support dynamic libraries"));
	#else
				auto path = (const char*)registers[pc->a];
		#ifndef MIZU_NO_EXCEPTIONS
				try {
					registers[pc->out] = (size_t)loader::load_shared(path, false);
				} catch(loader::error) {
					registers[pc->out] = (size_t)loader::load_shared(path, true);
				}
		#else
				auto lib = loader::load_shared(path, false);
				if(!lib) lib = loader::load_shared(path, true);
				registers[pc->out] = (size_t)lib;
		#endif
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(load_library);

			/**
			 * Expects a list of null terminated c-string library paths in the first \p immediate argument registers.
			 * Tries to load each library and stores a pointer to the first to load in \p out
			 * 
			 * @param out Register to store the resulting library in.
			 * @param immediate number of argument (a0, a1, etc...) registers storing paths to search
			 */
			void* load_first_library_that_exists(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
	#ifdef MIZU_WASM
				MIZU_THROW(std::runtime_error("WASM doesn't currently support dynamic libraries"));
	#else
				auto size = (uint32_t&)pc->a;
				std::array<std::string_view, 126> paths;
				for(size_t i = 0; i < size; ++i)
					paths[i] = {(char*)registers[a(i)]};

		#ifndef MIZU_NO_EXCEPTIONS
				try {
					registers[pc->out] = (size_t)loader::load_first_that_exists({paths.data(), size}, false);
				} catch(loader::error) {
					registers[pc->out] = (size_t)loader::load_first_that_exists({paths.data(), size}, true);
				}
		#else
				auto lib = loader::load_first_that_exists({paths.data(), size}, false);
				if(!lib) lib = loader::load_first_that_exists({paths.data(), size}, true);
				registers[pc->out] = (size_t)lib;
		#endif
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(load_first_library_that_exists);

			/**
			 * Loads a function pointer from a library.
			 * 
			 * @param out Register to store the resulting function pointer in
			 * @param a Register storing a pointer to the library to load the function from.
			 * @param b Register storing a null-terminated c-string pointer representing the (mangled) name of the function to load.
			 */
			void* load_library_function(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
	#ifdef MIZU_IMPLEMENTATION
			{
				auto lib = (loader::library*)registers[pc->a];
	#ifdef MIZU_WASM
				if(lib != nullptr) MIZU_THROW(std::runtime_error("WASM doesn't support finding functions outside the current executable!"));
	#else
				auto name = (const char*)registers[pc->b];
				registers[pc->out] = (size_t)loader::lookup(name, lib);
	#endif
				MIZU_NEXT();
			}
	#else // MIZU_IMPLEMENTATION
			;
	#endif // MIZU_IMPLEMENTATION
			MIZU_REGISTER_INSTRUCTION_PROTOTYPE(load_library_function);
		}}}

		MIZU_REGISTER_INSTRUCTION(ffi::push_type_void);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_pointer);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_i32);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_u32);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_i64);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_u64);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_f32);
		MIZU_REGISTER_INSTRUCTION(ffi::push_type_f64);
		MIZU_REGISTER_INSTRUCTION(ffi::clear_type_stack);
		MIZU_REGISTER_INSTRUCTION(ffi::create_interface);
		MIZU_REGISTER_INSTRUCTION(ffi::free_interface);
		MIZU_REGISTER_INSTRUCTION(ffi::call);
		MIZU_REGISTER_INSTRUCTION(ffi::call_with_return);
		MIZU_REGISTER_INSTRUCTION(ffi::load_library);
		MIZU_REGISTER_INSTRUCTION(ffi::load_first_library_that_exists);
		MIZU_REGISTER_INSTRUCTION(ffi::load_library_function);
	}

#endif // MIZU_NO_FFI