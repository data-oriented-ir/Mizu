#pragma once

#include "../mizu/opcode.hpp"

#include <cmath>
#include <concepts>

namespace mizu {
	using float32_t = float;
	using float64_t = double;

	template<std::floating_point F>
	inline F& float_register(uint64_t* registers, uint16_t index) {
		return (F&)registers[index];
	}

	inline namespace instructions { extern "C" {

	#define MIZU_FLOAT_TYPE(bits) float ## bits ## _t
	#define MIZU_FLOAT_CONVERSION(bits_from, bits_to)\
		void* convert_f ## bits_from ## _to_f ## bits_to(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits_to)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits_from)>(registers, pc->a);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(convert_f ## bits_from ## _to_f ## bits_to);

		MIZU_FLOAT_CONVERSION(32, 64)
		MIZU_FLOAT_CONVERSION(64, 32)


#ifdef MIZU_IMPLEMENTATION
	#define MIZU_FLOAT_INSTRUCTIONS(bits)\
		void* stack_load_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = *(MIZU_FLOAT_TYPE(bits)*)(sp + registers[pc->a]);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(stack_load_f ## bits);\
\
		void* stack_store_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = *(MIZU_FLOAT_TYPE(bits)*)(sp + registers[pc->a]) = (MIZU_FLOAT_TYPE(bits)&)registers[pc->b];\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(stack_store_f ## bits);\
\
		void* convert_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = registers[pc->a];\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(convert_to_f ## bits);\
\
		void* convert_signed_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = (int64_t&)registers[pc->a];\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(convert_signed_to_f ## bits);\
\
		void* convert_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(convert_from_f ## bits);\
\
		void* convert_signed_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = (int64_t&)registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(convert_signed_from_f ## bits);\
\
		void* add_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) + float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(add_f ## bits);\
\
		void* subtract_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) - float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(subtract_f ## bits);\
\
		void* multiply_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) * float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(multiply_f ## bits);\
\
		void* divide_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) / float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(divide_f ## bits);\
\
		void* max_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = std::max(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a), float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(max_f ## bits);\
\
		void* min_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = std::min(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a), float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(min_f ## bits);\
\
		void* sqrt_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = std::sqrt(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(sqrt_f ## bits);\
\
\
		void* set_if_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) == float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_equal_f ## bits);\
\
		void* set_if_not_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) != float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_not_equal_f ## bits);\
\
		void* set_if_less_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) < float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_less_f ## bits);\
\
		void* set_if_greater_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) >= float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_greater_equal_f ## bits);\
\
\
		void* set_if_negative_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = std::signbit(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_negative_f ## bits);\
\
		void* set_if_positive_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = !std::signbit(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_positive_f ## bits);\
\
		void* set_if_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = std::isinf(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_infinity_f ## bits);\
\
		void* set_if_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {\
			auto dbg = registers[pc->out] = std::isnan(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			MIZU_NEXT();\
		}\
		MIZU_REGISTER_INSTRUCTION(set_if_nan_f ## bits);
#else
	#define MIZU_FLOAT_INSTRUCTIONS(bits)\
		void* stack_load_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* stack_store_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* convert_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* convert_signed_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* convert_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* convert_signed_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* add_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* subtract_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* multiply_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* divide_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* max_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* min_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* sqrt_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
\
		void* set_if_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_not_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_less_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_greater_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
\
		void* set_if_negative_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_positive_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_negative_normal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_positive_normal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_negative_subnormal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_positive_subnormal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_negative_zero_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_positive_zero_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_negative_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);\
		void* set_if_positive_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp);
#endif


		MIZU_FLOAT_INSTRUCTIONS(32)
		MIZU_FLOAT_INSTRUCTIONS(64)
	}}
}