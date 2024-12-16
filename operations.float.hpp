#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdio>

#include "opcode.hpp"

namespace mizu {
	using float32_t = float;
	using float64_t = double;

	template<std::floating_point F>
	inline F& float_register(uint64_t* registers, uint16_t index) {
		return (F&)registers[index];
	}

	inline namespace operations { extern "C" {

	#define MIZU_FLOAT_TYPE(bits) float ## bits ## _t
	#define MIZU_FLOAT_CONVERSION(bits_from, bits_to)\
		void* convert_f ## bits_from ## _to_f ## bits_to(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits_to)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits_from)>(registers, pc->a);\
			MIZU_NEXT();\
		}

		MIZU_FLOAT_CONVERSION(32, 64)
		MIZU_FLOAT_CONVERSION(64, 32)


#ifdef MIZU_IMPLEMENTATION
	#define MIZU_FLOAT_OPERATIONS(bits)\
		void* stack_load_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = *(MIZU_FLOAT_TYPE(bits)*)(sp + registers[pc->a]);\
			MIZU_NEXT();\
		}\
\
		void* stack_store_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = *(MIZU_FLOAT_TYPE(bits)*)(sp + registers[pc->a]) = (MIZU_FLOAT_TYPE(bits)&)registers[pc->b];\
			MIZU_NEXT();\
		}\
\
		void* convert_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = registers[pc->a];\
			MIZU_NEXT();\
		}\
\
		void* convert_signed_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = (int64_t&)registers[pc->a];\
			MIZU_NEXT();\
		}\
\
		void* convert_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a);\
			MIZU_NEXT();\
		}\
\
		void* convert_signed_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			(int64_t&)registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a);\
			MIZU_NEXT();\
		}\
\
		void* add_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) + float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* subtract_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) - float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* multiply_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) * float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* divide_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) / float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* max_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = std::max(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a), float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b));\
			MIZU_NEXT();\
		}\
\
		void* min_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = std::min(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a), float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b));\
			MIZU_NEXT();\
		}\
\
		void* sqrt_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->out) = std::sqrt(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			MIZU_NEXT();\
		}\
\
\
		void* set_if_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) == float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* set_if_not_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) != float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* set_if_less_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) < float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
		void* set_if_greater_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			registers[pc->out] = float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a) >= float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->b);\
			MIZU_NEXT();\
		}\
\
\
		inline std::pair<int, bool> classify_f ## bits(MIZU_FLOAT_TYPE(bits) f) {\
			return {/*classify*/std::fpclassify(f), /*negative*/std::signbit(f)};\
		}\
		void* set_if_negative_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_INFINITE && negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_positive_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_INFINITE && !negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_negative_normal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_NORMAL && negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_positive_normal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_NORMAL && !negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_negative_subnormal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_SUBNORMAL && negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_positive_subnormal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_SUBNORMAL && !negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_negative_zero_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_ZERO && negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_positive_zero_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_ZERO && !negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_negative_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_NAN && negative;\
			MIZU_NEXT();\
		}\
\
		void* set_if_positive_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {\
			auto [classify, negative] = classify_f ## bits(float_register<MIZU_FLOAT_TYPE(bits)>(registers, pc->a));\
			registers[pc->out] = classify == FP_NAN && !negative;\
			MIZU_NEXT();\
		}
#else
	#define MIZU_FLOAT_OPERATIONS(bits)\
		void* stack_load_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* stack_store_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* convert_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* convert_signed_to_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* convert_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* convert_signed_from_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* add_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* subtract_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* multiply_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* divide_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* max_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* min_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* sqrt_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
\
		void* set_if_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_not_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_less_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_greater_equal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
\
		void* set_if_negative_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_positive_infinity_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_negative_normal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_positive_normal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_negative_subnormal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_positive_subnormal_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_negative_zero_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_positive_zero_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_negative_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);\
		void* set_if_positive_nan_f ## bits(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);
#endif


		MIZU_FLOAT_OPERATIONS(32)
		MIZU_FLOAT_OPERATIONS(64)
	}}
}