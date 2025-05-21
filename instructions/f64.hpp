#pragma once

#include "f32.hpp"

namespace mizu { inline namespace instructions { extern "C" {
	/**
	 * Converts the provided f32 register to an f64 register
	 * 
	 * @param out Register to store the f64 result in
	 * @param a Register storing the f32 to convert
	 */
	void* convert_f32_to_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		float_register<std::float64_t>(registers, pc->out) = float_register<std::float32_t>(registers, pc->a);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(convert_f32_to_f64);

	/**
	 * Converts the provided f64 register to an f32 register
	 * 
	 * @param out Register to store the f32 result in
	 * @param a Register storing the f64 to convert
	 */
	void* convert_f64_to_f32(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		float_register<std::float32_t>(registers, pc->out) = float_register<std::float64_t>(registers, pc->a);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(convert_f64_to_f32);

// 	void* stack_load_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
// #ifdef MIZU_IMPLEMENTATION
// 	{
// 		float_register<std::float32_t>(registers, pc->out) = *(std::float32_t*)(sp + registers[pc->a]);
// 		MIZU_NEXT();
// 	}
// #else
// 	;
// #endif
// 	MIZU_REGISTER_INSTRUCTION(stack_load_f64);

// 	void* stack_store_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
// #ifdef MIZU_IMPLEMENTATION
// 	{
// 		float_register<std::float32_t>(registers, pc->out) = *(std::float32_t*)(sp + registers[pc->a]) = (std::float32_t&)registers[pc->b];
// 		MIZU_NEXT();
// 	}
// #else
// 	;
// #endif
// 	MIZU_REGISTER_INSTRUCTION(stack_store_f64);

	/**
		* Converts the provided register into a float register
		* 
		* @param out Register to store the result of the conversion in
		* @param a Register to convert to an f64
		*/
	void* convert_to_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		float_register<std::float32_t>(registers, pc->out) = registers[pc->a];
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(convert_to_f64);

	/**
		* Converts the provided (signed) register into a float register
		* 
		* @param out Register to store the result of the conversion in
		* @param a Register to convert to an f64
		*/
	void* convert_signed_to_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		float_register<std::float32_t>(registers, pc->out) = (int64_t&)registers[pc->a];
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(convert_signed_to_f64);

	/**
		* Truncates the provided float register and stores it in the given register (as an unsigned number)
		* 
		* @param out Register to store the result of the conversion in
		* @param a Float register to convert to a u64
		*/
	void* convert_from_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = float_register<std::float32_t>(registers, pc->a);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(convert_from_f64);

	/**
		* Truncates the provided float register and stores it in the given register
		* 
		* @param out Register to store the result of the conversion in
		* @param a Float register to convert to an i64
		*/
	void* convert_signed_from_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = (int64_t&)registers[pc->out] = float_register<std::float32_t>(registers, pc->a);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(convert_signed_from_f64);

	/**
		* Adds two f64 numbers
		* @param out register to store \p a + \p b in
		* @param a register storing first value
		* @param b register storing second value
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* add_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = float_register<std::float32_t>(registers, pc->a) + float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(add_f64);

	/**
		* Subtracts two f64 numbers
		* @param out register to store \p a - \p b in
		* @param a register storing first value
		* @param b register storing second value
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* subtract_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = float_register<std::float32_t>(registers, pc->a) - float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(subtract_f64);

	/**
		* Multiplies two f64 numbers
		* @param out register to store \p a * \p b in
		* @param a register storing first value
		* @param b register storing second value
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* multiply_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = float_register<std::float32_t>(registers, pc->a) * float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(multiply_f64);

	/**
		* Divides two f64 numbers
		* @param out register to store \p a / \p b in
		* @param a register storing first value
		* @param b register storing second value
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* divide_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = float_register<std::float32_t>(registers, pc->a) / float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(divide_f64);

	/**
		* Finds the max of two f64 numbers
		* @param out register to store max( \p a, \p b ) in
		* @param a register storing first value
		* @param b register storing second value
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* max_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = std::max(float_register<std::float32_t>(registers, pc->a), float_register<std::float32_t>(registers, pc->b));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(max_f64);

	/**
		* Finds the min of two f64 numbers
		* @param out register to store min( \p a, \p b ) in
		* @param a register storing first value
		* @param b register storing second value
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* min_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION 
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = std::min(float_register<std::float32_t>(registers, pc->a), float_register<std::float32_t>(registers, pc->b));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(min_f64);

	/**
		* Finds the square root of a f64 number
		* @param out register to store sqrt( \p a ) in
		* @param a register storing the value
		*/
	void* sqrt_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = float_register<std::float32_t>(registers, pc->out) = std::sqrt(float_register<std::float32_t>(registers, pc->a));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(sqrt_f64);

	/**
		* Checks if two f64 registers are equal
		* @param out register to be set to one if \p a == \p b or zero otherwise
		* @param a register storing the first value to compare
		* @param b register storing the second value to compare
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* set_if_equal_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = float_register<std::float32_t>(registers, pc->a) == float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_equal_f64);

	/**
		* Checks if two f64 registers are not equal
		* @param out register to be set to one if \p a != \p b or zero otherwise
		* @param a register storing the first value to compare
		* @param b register storing the second value to compare
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* set_if_not_equal_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = float_register<std::float32_t>(registers, pc->a) != float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_not_equal_f64);

	/**
		* Checks if one f64 register is less than another
		* @param out register to be set to one if \p a < \p b or zero otherwise
		* @param a register storing the first value to compare
		* @param b register storing the second value to compare
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* set_if_less_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = float_register<std::float32_t>(registers, pc->a) < float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_less_f64);

	/**
		* Checks if one f64 register is greater or equal to another
		* @param out register to be set to one if \p a >= \p b or zero otherwise
		* @param a register storing the first value to compare
		* @param b register storing the second value to compare
		* @note Both \p a and \p b must be f64s. If they aren't they should be converted first
		*/
	void* set_if_greater_equal_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = float_register<std::float32_t>(registers, pc->a) >= float_register<std::float32_t>(registers, pc->b);
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_greater_equal_f64);


	/**
		* Checks if a f64 register is negative
		* @param out register to be set to one if \p a < 0 or zero otherwise
		* @param a register storing the value to compare
		*/
	void* set_if_negative_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = std::signbit(float_register<std::float32_t>(registers, pc->a));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_negative_f64);

	/**
		* Checks if a f64 register is positive
		* @param out register to be set to one if \p a > 0 or zero otherwise
		* @param a register storing the value to compare
		*/
	void* set_if_positive_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = !std::signbit(float_register<std::float32_t>(registers, pc->a));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_positive_f64);

	/**
		* Checks if a f64 register is an infinity
		* @param out register to be set to one if \p a == +/- infinity or zero otherwise
		* @param a register storing the value to compare
		*/
	void* set_if_infinity_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = std::isinf(float_register<std::float32_t>(registers, pc->a));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_infinity_f64);

	/**
		* Checks if a f64 register is nan
		* @param out register to be set to one if \p a == nan or zero otherwise
		* @param a register storing the value to compare
		*/
	void* set_if_nan_f64(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp) 
#ifdef MIZU_IMPLEMENTATION
	{
		auto dbg = registers[pc->out] = std::isnan(float_register<std::float32_t>(registers, pc->a));
		MIZU_NEXT();
	}
#else
	;
#endif
	MIZU_REGISTER_INSTRUCTION(set_if_nan_f64);
}}}