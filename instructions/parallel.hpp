#pragma once

#include "../mizu/opcode.hpp"
#include <algorithm>
#include <chrono>

#ifndef MIZU_NO_HARDWARE_THREADS
	#include "../mizu/exception.hpp"

	#include <thread>
	#include <msd/channel.hpp> // TODO: Need to implement non
#endif

namespace mizu {
	// NOTE: Not a valid instruction
	inline uint64_t new_thread(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {
#ifndef MIZU_NO_HARDWARE_THREADS
		registers_and_stack env;
		std::copy(registers, registers + memory_size, env.memory.data());

		return (uint64_t)new std::thread([pc, env = std::move(env)]() mutable {
			setup_environment(env);
			return pc->op(pc, env.memory.data(), env.stack_boundary, env.stack_pointer);
		});
#else // MIZU_NO_HARDWARE_THREADS
		auto env = (registers_and_stack*)malloc(sizeof(registers_and_stack));
		std::copy(registers, registers + memory_size, env->memory.data());
		setup_environment(*env);
		mizu::coroutine::start(pc, env);
		return fpda_size(mizu::coroutine::contexts) - 1; // Return the index of the thread in the context
#endif // MIZU_NO_HARDWARE_THREADS
	}

#ifndef MIZU_CUSTOM_DELAY_IMPLEMENTATION
	inline void delay(std::chrono::microseconds time, opcode*& pc, uint64_t& storage_register) {
	#ifndef MIZU_NO_HARDWARE_THREADS
		std::this_thread::sleep_for(time);
	#else
		if(!storage_register) storage_register = (size_t)new std::chrono::high_resolution_clock::time_point(std::chrono::high_resolution_clock::now());

		auto start = (std::chrono::high_resolution_clock::time_point*)storage_register;
		if(std::chrono::high_resolution_clock::now() - *start > time)
			--pc;
		else delete start;
	#endif
	}
#else
	extern void delay(std::chrono::microseconds time, opcode*& pc, uint64_t& storage_register);
#endif

	inline namespace instructions { extern "C" {

		/**
		 * Forks a new thread moving its program counter by an offset relative to the current thread's program counter. If the offset is zero this instruction is executed again. If it is one the next instruction is executed as usual.
		 *	If it is negative a previous instruction is executed.
		 * @note A new set of registers (copied from the current thread) are created for the new thread.
		 *
		 * @param out register to store the thread reference in
		 * @param a register storing how many instructions to jump
		 * @note \p a is interpreted as a signed integer, allowing for negative jumps
		 */
		void* fork_relative(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto threadPC = pc + *(int64_t*)&registers[pc->a];
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack_boundary, sp);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(fork_relative);

		/**
		 * Forks a new thread moving its program counter by an offset relative to the current thread's program counter. If the offset is zero this instruction is executed again. If it is one the next instruction is executed as usual.
		 *	If it is negative a previous instruction is executed.
		 * @note A new set of registers (copied from the current thread) are created for the new thread.
		 *
		 * @param out register to store the thread reference in
		 * @param immediate how many instructions to jump
		 * @note \p immediate is interpreted as a signed integer, allowing for negative jumps
		 */
		void* fork_relative_immediate(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto threadPC = pc + *(int32_t*)&pc->a;
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack_boundary, sp);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(fork_relative_immediate);

		/**
		 * Forks a new thread with its program counter set to a value (usually the output of another jump)
		 * @note A new set of registers (copied from the current thread) are created for the new thread.
		 *
		 * @param out register to store the thread reference in
		 * @param a register storing the address of the instruction to jump to.
		 */
		void* fork_to(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto threadPC = (opcode*)registers[pc->a];
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack_boundary, sp);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(fork_to);

		/**
		 * Waits for the provided thread to finish and frees its reference
		 * 
		 * @param a Register storing the thread to wait for
		 * @param b Register storing a value to overwrite \p a with (defaults to zero)
		 */
		void* join_thread(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
	#ifndef MIZU_NO_HARDWARE_THREADS
			std::thread* thread = (std::thread*)registers[pc->a];
			if(thread) {
				if(thread->joinable()) thread->join();
				delete thread;
				registers[pc->a] = registers[pc->b];
			}
	#else // MIZU_NO_HARDWARE_THREADS
			size_t context_id = (size_t&)registers[pc->a];
			if(context_id) { // Can't join the main thread!
				if(!mizu::coroutine::get_context(context_id).done())
					// If the thread in question is not done yet, back up the program counter (aka queue this same instruction up to be run next)
					--pc;
				else registers[pc->a] = registers[pc->b];
			}
	#endif // MIZU_NO_HARDWARE_THREADS
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(join_thread);

		/**
		 * Sleeps the provided number of micro seconds
		 * 
		 * @param a Register storing the number of microseconds to sleep
		 */
		void* sleep_microseconds(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto time = std::chrono::microseconds(registers[pc->a]);
			delay(time, pc, registers[pc->out]);
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(sleep_microseconds);


		/**
		 * Creates an inter-thread communication channel. This channel can keep a buffer of values or a single value.
		 * @note Channels send and receive register sized binary blobs.
		 * 
		 * @param out Register to store the channel reference in
		 * @param a Register storing the capacity of the channel buffer (defaults to a single item)
		 */
		void* channel_create(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto capacity = registers[pc->a];
	#ifndef MIZU_NO_HARDWARE_THREADS
			registers[pc->out] = (uint64_t)new msd::channel<uint64_t>(capacity);
	#else // MIZU_NO_HARDWARE_THREADS
			auto channel = fp::dynarray<uint64_t>(nullptr);
			registers[pc->out] = (uint64_t)channel.reserve(std::max<size_t>(1, capacity)).raw;
	#endif // MIZU_NO_HARDWARE_THREADS
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(channel_create);

		/**
		 * Closes the provided channel and frees its reference
		 * 
		 * @param a Register storing the channel to free
		 * @param b Register storing a value to overwrite \p a with (defaults to zero)
		 */
		void* channel_close(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
	#ifndef MIZU_NO_HARDWARE_THREADS
			auto channel = (msd::channel<uint64_t>*)registers[pc->a];
			if(!channel) MIZU_THROW(std::runtime_error("Channel does not exist."));
			delete channel;
	#else // MIZU_NO_HARDWARE_THREADS
			auto channel = fp::dynarray<uint64_t>((uint64_t*)registers[pc->a]);
			// if(!channel) MIZU_THROW(std::runtime_error("Channel does not exist."));
			channel.free();
	#endif // MIZU_NO_HARDWARE_THREADS
			registers[pc->a] = registers[pc->b];
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(channel_close);

		/**
		 * Blocks the current thread until there is an item to receive from the channel.
		 * 
		 * @param out Register to store the binary blob read from the channel in.
		 * @param a Register storing the channel to read from.
		 */
		void* channel_receive(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
	#ifndef MIZU_NO_HARDWARE_THREADS
			auto channel = (msd::channel<uint64_t>*)registers[pc->a];
			if(!channel) MIZU_THROW(std::runtime_error("Channel does not exist."));
			*channel >> registers[pc->out];
	#else // MIZU_NO_HARDWARE_THREADS
			auto channel = fp::dynarray<uint64_t>((uint64_t*)registers[pc->a]);
			// if(!channel) MIZU_THROW(std::runtime_error("Channel does not exist."));
			if(channel.empty()) {
				// If there is nothing to receive, nudge back the program counter
				--pc;
			} else registers[pc->out] = channel.pop_front();
	#endif // MIZU_NO_HARDWARE_THREADS
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(channel_receive);

		/**
		 * Blocks the current thread until the channel has room to receive a new value.
		 * 
		 * @param a Register storing the channel to send a message to
		 * @param b Register storing the binary blob to send to the channel.
		 */
		void* channel_send(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
	#ifndef MIZU_NO_HARDWARE_THREADS
			auto channel = (msd::channel<uint64_t>*)registers[pc->a];
			if(!channel) MIZU_THROW(std::runtime_error("Channel does not exist."));
			*channel << registers[pc->b];
	#else // MIZU_NO_HARDWARE_THREADS
			auto channel = fp::dynarray<uint64_t>((uint64_t*)registers[pc->a]);
			// if(!channel) MIZU_THROW(std::runtime_error("Channel does not exist."));
			if(channel.size() == channel.capacity()) {
				// If the channel is full, nudge back the program counter
				--pc;
			} else channel.push_back(registers[pc->b]);
	#endif // MIZU_NO_HARDWARE_THREADS
			MIZU_NEXT();
		}
#else
		;
#endif
		MIZU_REGISTER_INSTRUCTION(channel_send);
	}}
}