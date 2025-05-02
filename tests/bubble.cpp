#define MIZU_NO_HARDWARE_THREADS
#define MIZU_IMPLEMENTATION
#include <mizu/instructions.hpp>

const fp::array<uint64_t, 100> numbers = {
	179, 1630, 754, 259, 858, 970, 310, 1612, 1269, 1000, 397, 783, 814, 1812, 1778, 641, 1925, 382, 82, 1147,
	152, 399, 1061, 1364, 1323, 1753, 96, 980, 1849, 1155, 1355, 1558, 168, 982, 1659, 598, 8, 1547, 52, 1164,
	1555, 445, 1069, 1921, 627, 1337, 845, 193, 1829, 1572, 1681, 1885, 197, 894, 1940, 1081, 1839, 313, 26, 116,
	692, 1105, 489, 1293, 502, 1019, 567, 496, 787, 1757, 1333, 1863, 1291, 1975, 744, 457, 1113, 1974, 246, 164,
	1441, 854, 1710, 583, 648, 484, 1279, 1890, 1588, 1073, 1944, 1231, 656, 566, 1676, 301, 1931, 667, 1167, 707
};
const fp::array<uint64_t, 100> sorted = {
	8, 26, 52, 82, 96, 116, 152, 164, 168,179, 193, 197, 246, 259, 301, 310, 313, 382, 397, 399, 445, 457, 484, 489, 
	496, 502, 566, 567, 583, 598, 627, 641, 648, 656, 667, 692, 707, 744, 754, 783, 787, 814, 845, 854, 858, 894, 970, 
	980, 982, 1000, 1019, 1061, 1069, 1073, 1081, 1105, 1113, 1147, 1155, 1164, 1167, 1231, 1269, 1279, 1291, 1293, 
	1323, 1333, 1337, 1355, 1364, 1441, 1547, 1555, 1558, 1572, 1588, 1612, 1630, 1659, 1676, 1681, 1710, 1753, 1757, 
	1778, 1812, 1829, 1839, 1849, 1863, 1885, 1890, 1921, 1925, 1931, 1940, 1944, 1974, 1975
};

MIZU_MAIN() {
	using namespace mizu;

	const static opcode bubble_program[] = {
		opcode{find_label, 200}.set_immediate(label2int("bub")), // while loop
		opcode{find_label, 201}.set_immediate(label2int("inner")), // for loop
		opcode{find_label, 202}.set_immediate(label2int("check")), // loop end
		opcode{find_label, 203}.set_immediate(label2int("ctop")), // check/assert loop
		opcode{load_immediate, 204}.set_immediate(sizeof(uint64_t)), // type size constant
		// opcode{find_label, 203}.set_immediate(label2int("end")), // check/assert loop
		// a0 (size) = 100
		opcode{load_immediate, registers::a(0)}.set_immediate(100),
		// sp = numbers
		opcode{multiply, registers::t(0), 204, registers::a(0)}, // 204 == sizeof(uint64_t)
		opcode{stack_push, 0, registers::t(0)},
		opcode{unsafe::pointer_to_stack, registers::t(1)},
		opcode{load_immediate, registers::t(2)}.set_host_pointer_lower_immediate(numbers.data()),
		opcode{load_upper_immediate, registers::t(2)}.set_host_pointer_upper_immediate(numbers.data()),
		opcode{unsafe::copy_memory, registers::t(1), registers::t(2), registers::t(0)},
		// Bubble Sort 
		// a1 (changed) = true
		opcode{load_immediate, registers::a(1)}.set_immediate(1),
		opcode{label}.set_immediate(label2int("bub")),
			// if not a1 (changed) jump out
			opcode{set_if_equal, registers::t(0), registers::a(1), 0},
			opcode{branch_to, 0, registers::t(0), 202}, // 202 -> goto check
			// a1 (changed) = false
			opcode{load_immediate, registers::a(1)}.set_immediate(0),
			// a2 (i) = 0
			opcode{load_immediate, registers::a(2)}.set_immediate(0),
				// Inner loop
				opcode{label}.set_immediate(label2int("inner")),
				// if a2 (i) >= a0 (size) jump up
				opcode{set_if_greater_equal, registers::t(0), registers::a(2), registers::a(0)},
				opcode{branch_to, 0, registers::t(0), 200}, // 200 -> goto while loop
				// t0 = a2 (a2 - 1 in terms of the next ops)
				opcode{add, registers::t(0), registers::a(2), 0},
				// a2 (i) += 1
				opcode{load_immediate, registers::t(1)}.set_immediate(1),
				opcode{add, registers::a(2), registers::a(2), registers::t(1)},
				// t0 = sp[t0] (i - 1), t2 = offset
				// opcode{load_immediate, registers::t(1)}.set_immediate(sizeof(uint64_t)),
				opcode{multiply, registers::t(2), registers::t(0), 204}, // 204 == sizeof(uint64_t)
				opcode{stack_load_u64, registers::t(0), registers::t(2)},
				// t1 = sp[a2] (i), t3 = offset
				// opcode{load_immediate, registers::t(1)}.set_immediate(sizeof(uint64_t)),
				opcode{multiply, registers::t(3), registers::a(2), 204}, // 204 == sizeof(uint64_t)
				opcode{stack_load_u64, registers::t(1), registers::t(3)},
					// if t0 (sp[i - 1]) <= t1 (sp[i]) continue
					opcode{set_if_greater_equal, registers::t(4), registers::t(1), registers::t(0)},
					opcode{branch_to, 0, registers::t(4), 201}, // 201 -> goto top of for
					// sp[t2] = t1, sp[t3] = t0
					opcode{stack_store_u64, 0, registers::t(1), registers::t(2)},
					opcode{stack_store_u64, 0, registers::t(0), registers::t(3)},
					// a1 (changed) = true
					opcode{load_immediate, registers::a(1)}.set_immediate(1),
					// continue
					opcode{jump_to, 0, 201}, // 201 -> goto top of for
	
		// Assert all equal
		opcode{label}.set_immediate(label2int("check")),
		// sp = sorted
		// opcode{load_immediate, registers::t(0)}.set_immediate(sizeof(uint64_t)),
		opcode{multiply, registers::t(0), 204, registers::a(0)}, // 204 == sizeof(uint64_t)
		opcode{stack_push, 0, registers::t(0)},
		opcode{unsafe::pointer_to_stack, registers::t(1)},
		opcode{load_immediate, registers::t(2)}.set_host_pointer_lower_immediate(sorted.data()),
		opcode{load_upper_immediate, registers::t(2)}.set_host_pointer_upper_immediate(sorted.data()),
		opcode{unsafe::copy_memory, registers::t(1), registers::t(2), registers::t(0)},
		// a1 (i) = 0
		opcode{load_immediate, registers::a(1)}.set_immediate(0),
		// Assert loop
		opcode{label}.set_immediate(label2int("ctop")),
			// if a1 (i) >= a0 (size) halt
			opcode{set_if_less, registers::t(0), registers::a(1), registers::a(0)},
			opcode{branch_relative_immediate, 0}.set_branch_immediate(2),
			opcode{halt},
			// t0 = sp[a1], t1 = offset
			// opcode{load_immediate, registers::t(2)}.set_immediate(sizeof(uint64_t)),
			opcode{multiply, registers::t(1), registers::a(1), 204}, // 204 == sizeof(uint64_t)
			opcode{stack_load_u64, registers::t(0), registers::t(1)},
			// t1 = (sp + size * sizeof(uint64_t))[a1]
			opcode{multiply, registers::t(2), registers::a(0), 204}, // 204 == sizeof(uint64_t)
			opcode{add, registers::t(1), registers::t(1), registers::t(2)},
			opcode{stack_load_u64, registers::t(1), registers::t(1)},
			// a1 (i) += 1
			opcode{load_immediate, registers::t(2)}.set_immediate(1),
			opcode{add, registers::a(1), registers::a(1), registers::t(2)},
			// if t0 == t1 continue
			opcode{set_if_equal, registers::t(0), registers::t(0), registers::t(1)},
			opcode{branch_to, 0, registers::t(0), 203}, // 203 -> ctop (top of assert loop)
			// assert index (print a1)
			opcode{subtract, registers::t(0), registers::a(1), registers::t(2)},
			opcode{debug_print, 0, registers::t(0)},
			opcode{halt},
	};

	{
		registers_and_stack env = {};
		setup_environment(env);

		MIZU_START_FROM_ENVIRONMENT(program, env);
	}

	return 0;
}