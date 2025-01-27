#define MIZU_NO_HARDWARE_THREADS
#define MIZU_IMPLEMENTATION
#include <operations.hpp>

MIZU_MAIN() {
	using namespace mizu;

	const static opcode program[] = {
		opcode{find_label, 200}.set_immediate(label2int("fib")),
		// Mizu call (a0 = fib(40))
		opcode{load_immediate, registers::a(0)}.set_immediate(40),
		opcode{jump_to, registers::return_address, 200},
		opcode{debug_print, 0, registers::a(0)},
		opcode{halt},


		// Recursive Fibonachi
		opcode{label}.set_immediate(label2int("fib")),
		// if(a0 >= 3) skip return 1
		opcode{load_immediate, registers::t(0)}.set_immediate(3),
		opcode{set_if_greater_equal, registers::t(0), registers::a(0), registers::t(0)},
		opcode{branch_relative_immediate, 0, registers::t(0)}.set_branch_immediate(3),
		// return 1
		opcode{load_immediate, registers::a(0)}.set_immediate(1),
		opcode{jump_to, 0, registers::return_address}, // return
		// save ra, save a2, save a3
		opcode{stack_push_immediate, 0}.set_immediate(24),
		opcode{load_immediate, registers::t(0)}.set_immediate(24),
		opcode{stack_store_u64, 0, registers::t(0), registers::return_address},
		opcode{load_immediate, registers::t(0)}.set_immediate(16),
		opcode{stack_store_u64, 0, registers::t(0), registers::a(2)},
		opcode{load_immediate, registers::t(0)}.set_immediate(8),
		opcode{stack_store_u64, 0, registers::t(0), registers::a(3)},
		// a2 = a0 - 1
		opcode{load_immediate, registers::t(0)}.set_immediate(1),
		opcode{subtract, registers::a(2), registers::a(0), registers::t(0)},
		// a3 = a0 - 2
		opcode{load_immediate, registers::t(0)}.set_immediate(2),
		opcode{subtract, registers::a(3), registers::a(0), registers::t(0)},
		// a2 = fib(a2)
		opcode{add, registers::a(0), registers::a(2), 0},
		opcode{jump_to, registers::return_address, 200}, // 200 == fib
		opcode{add, registers::a(2), registers::a(0), 0},
		// a0 = fib(a3)
		opcode{add, registers::a(0), registers::a(3), 0},
		opcode{jump_to, registers::return_address, 200}, // 200 == fib
		// opcode{add, registers::a(3), registers::a(0), 0},
		// a0 = a2 + a0
		opcode{add, registers::a(0), registers::a(2), registers::a(0)},
		// restore ra, a2, a3
		opcode{load_immediate, registers::t(0)}.set_immediate(24),
		opcode{stack_load_u64, registers::return_address, registers::t(0)},
		opcode{load_immediate, registers::t(0)}.set_immediate(16),
		opcode{stack_load_u64, registers::a(2), registers::t(0)},
		opcode{load_immediate, registers::t(0)}.set_immediate(8),
		opcode{stack_load_u64, registers::a(3), registers::t(0)},
		opcode{stack_pop_immediate}.set_immediate(24),
		// return
		opcode{jump_to, 0, registers::return_address}, // return
	};

	{
		registers_and_stack env = {};
		setup_enviornment(env);

		MIZU_START_FROM_ENVIORNMENT(program, env);
	}

	return 0;
}