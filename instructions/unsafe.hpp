#pragma once

#include "../mizu/opcode.hpp"
#include <fp/pointer.h>

namespace mizu::unsafe {
	inline namespace instructions { extern "C" {

		/**
		 * Allocates new memory on the host application heap.
		 * 
		 * @param out Register in which a pointer to the allocated memory will be stored
		 * @param a Register storing how many bytes to allocate
		 */
		void* allocate(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			size_t n = registers[pc->a];
			registers[pc->out] = (size_t)malloc(n);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(allocate);

		/**
		 * Frees an allocation alloacted with \ref allocate.
		 * 
		 * @param a Register storing the allocation to free
		 * @param b Register storing a value to overwrite \p a with (defaults to zero)
		 */
		void* free_allocated(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			void* p = (void*)registers[pc->a];
			free(p);
			registers[pc->a] = registers[pc->b];
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(free_allocated);


		/**
		 * Allocates new memory on the host application heap. This memory knows how many element are stored in it.
		 * 
		 * @param out Register in which a pointer to the allocated memory will be stored
		 * @param a Register storing the size of each element
		 * @param b Register storing how many elements there are
		 * @note The total size of this allocation is a * b
		 */
		void* allocate_fat_pointer(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			size_t size = registers[pc->a];
			size_t n = registers[pc->b];
			registers[pc->out] = (size_t)__fp_malloc(size, n);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(allocate_fat_pointer);

		/**
		 * Frees an allocation alloacted with \ref allocate_fat_pointer.
		 * 
		 * @param a Register storing the allocation to free
		 * @param b Register storing a value to overwrite \p a with (defaults to zero)
		 */
		void* free_fat_pointer(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			void* p = (void*)registers[pc->a];
			fp_free(p);
			registers[pc->a] = registers[pc->b];
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(free_fat_pointer);

		/**
		 * Generates a pointer to memory on Mizu's stack.
		 * 
		 * @param out Register to store the pointer in
		 * @param a Register storing a (signed) offset from the current stack pointer.
		 */
		void* pointer_to_stack(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto offset = (int64_t&)registers[pc->a];
			registers[pc->out] = (size_t)(sp + offset);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(pointer_to_stack);

		/**
		 * Generates a pointer to one of Mizu's registers
		 * 
		 * @param a Register to take a pointer to.
		 * @param out Register to store the pointer in
		 */
		void* pointer_to_register(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			registers[pc->out] = (size_t)(registers + pc->a);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(pointer_to_register);

		/**
		 * Copies memory from one pointer to another
		 * 
		 * @param out Register storing a pointer that data should be copied to
		 * @param a Register storing a pointer that data should be copied from
		 * @param b Register storing how many bytes should be copied.
		 */
		void* copy_memory(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			size_t n = registers[pc->b];
			auto src = (void*)registers[pc->a];
			auto dest = (void*)registers[pc->out];
			std::memcpy(dest, src, n);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(copy_memory);
	}}
}