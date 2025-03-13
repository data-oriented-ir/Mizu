#pragma once

#include "../instructions/lookup.hpp"
#include "opcode.hpp"
#include <cassert>

namespace mizu { inline namespace serialization {
	/**
	 * Converts a Mizu \p program into a byte array ready to be written to a file or sent over the network.
	 * @note This function makes no account of different machine endianness or pointer sizes. 
	 *
	 * @param program The program to serialize
	 * @return fp::dynarray<std::byte> a dynamically allocated array of bytes representing the serialized program 
	 */
	inline fp::dynarray<std::byte> to_binary(fp::view<const opcode> program) {
		auto out = fp::dynarray<std::byte>{}.resize(program.size() * sizeof(opcode));
		std::memcpy(out.raw, program.data(), out.size());
		for(size_t i = 0, size = program.size(); i < size; ++i) {
			auto* op = ((opcode*)out.data()) + i;
			op->op = (operation_t)lookup_id(op->op).value_or(-1);
		}
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
		assert(binary.size() % sizeof(opcode) == 0);
		auto out = fp::dynarray<opcode>{}.resize(binary.size() / sizeof(opcode));
		std::memcpy(out.raw, binary.data(), binary.size());
		for(auto& op: out)
			op.op = lookup_pointer((size_t)op.op).value_or(nullptr);
		return out;
	}
}}