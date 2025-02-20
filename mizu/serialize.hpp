#pragma once

#include "../operations/lookup.hpp"
#include "opcode.hpp"

namespace mizu { inline namespace serialization {
	inline fp::dynarray<std::byte> to_binary(fp::view<const opcode> program) {
		auto out = fp::dynarray<std::byte>{}.resize(program.size() * sizeof(opcode));
		std::memcpy(out.raw, program.data(), out.size());
		for(size_t i = 0, size = program.size(); i < size; ++i) {
			auto* op = ((opcode*)out.data()) + i;
			op->op = (operation_t)lookup_id(op->op).value_or(-1);
		}
		return out;
	}
	inline fp::dynarray<std::byte> to_binary(fp::view<opcode> program) { return to_binary((fp::view<const opcode>&)program); }

	inline fp::dynarray<opcode> from_binary(fp::view<const std::byte> binary) {
		auto out = fp::dynarray<opcode>{}.resize(binary.size() / sizeof(opcode));
		std::memcpy(out.raw, binary.data(), binary.size());
		for(auto& op: out)
			op.op = lookup_pointer((size_t)op.op).value_or(nullptr);
		return out;
	}
	inline fp::dynarray<opcode> from_binary(fp::view<std::byte> binary) { return from_binary((fp::view<const std::byte>&)binary); }
}}