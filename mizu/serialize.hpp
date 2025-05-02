#pragma once

#include "../instructions/lookup.hpp"
#include <cassert>
#include <bit>

namespace mizu { inline namespace serialization {
	/**
	 * A version of the opcode which has fixed sizes for serialization purposes.
	 */
	struct serialization_opcode {
		uint64_t op;
		reg_t out, a, b;

		static serialization_opcode from_opcode(const opcode& code) {
			serialization_opcode out;
			out.op = (size_t)code.op;
			std::memcpy(&out.out, &code.out, 3 * sizeof(reg_t));
			return out;
		}

		opcode to_opcode() {
			opcode out;
			out.op = (instruction_t)op;
			std::memcpy(&out.out, &this->out, 3 * sizeof(reg_t));
			return out;
		}

		void byteswap() {
			op = std::byteswap(op);
			out = std::byteswap(out);
			a = std::byteswap(a);
			b = std::byteswap(b);
		}
	};

	/**
	 * Converts a Mizu \p program into a byte array ready to be written to a file or sent over the network.
	 * @note This function makes no account of different machine endianness or pointer sizes. 
	 *
	 * @param program The program to serialize
	 * @return fp::dynarray<std::byte> a dynamically allocated array of bytes representing the serialized program 
	 */
	inline fp::dynarray<std::byte> to_binary(fp::view<const opcode> _program) {
		// Make sure the program is stored with 64bit ops.
		bool free_program = false;
		fp::view<serialization_opcode> program;
		if constexpr(sizeof(opcode) == sizeof(serialization_opcode))
			program = (fp::view<serialization_opcode>&)_program;
		else {
			free_program = true;
			auto temp = fp::dynarray<serialization_opcode>{nullptr}.reserve(_program.size());
			for(auto& opcode: _program)
				temp.push_back(serialization_opcode::from_opcode(opcode));
			program = temp.full_view();
		}

		// Replace the op pointers with ids from the lookup
		auto out = fp::dynarray<std::byte>{}.resize(program.size() * sizeof(serialization_opcode));
		std::memcpy(out.raw, program.data(), out.size());
		for(size_t i = 0, size = program.size(); i < size; ++i) {
			auto* op = ((serialization_opcode*)out.data()) + i;
			op->op = lookup_id((instruction_t)op->op).value_or(-1);
		}

		// Make sure all the integers are stored little endian
		if constexpr (std::endian::native != std::endian::little)
			for(auto& op: program)
				op.byteswap();

		if(free_program) fp::dynarray<serialization_opcode>{program.data()}.free();
		return out;
	}

	/**
	 * Converts a blob of \p binary data into a Mizu program
	 * @note This function makes no account of different machine endianness or pointer sizes. 
	 * 
	 * @param binary The binary blob to deserialize
	 * @return fp::dynarray<opcode> a dynamically allocated Mizu program
	 */
	inline fp::dynarray<opcode> from_binary(fp::view<const std::byte> binary) {
		assert(binary.size() % sizeof(serialization_opcode) == 0);
		auto out = fp::dynarray<serialization_opcode>{}.resize(binary.size() / sizeof(serialization_opcode));
		std::memcpy(out.raw, binary.data(), binary.size());

		// The binary data stores integers in little endian... if our machine is not then we need to swap
		if constexpr (std::endian::native != std::endian::little)
			for(auto& op: out)
				op.byteswap();

		// Lookup all the pointers
		for(auto& op: out)
			op.op = (size_t)lookup_pointer(op.op).value_or(nullptr);

		// If opcodes use 64bit pointers then we can just return 
		if constexpr(sizeof(opcode) == sizeof(serialization_opcode))
			return (fp::dynarray<opcode>&&)std::move(out);

		// Otherwise convert from 64bit serialization_opcodes to 32bit opcodes
		else {
			auto tmp = out;
			{
				auto out = fp::dynarray<opcode>{}.reserve(tmp.size());
				for(auto& opcode: tmp)
					out.push_back(opcode.to_opcode());
				tmp.free();
				return out;
			}
		}
	}
}}