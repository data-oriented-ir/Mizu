#pragma once

#include "../mizu/opcode.hpp"

namespace mizu {
	namespace debug { inline namespace instructions { 
		/**
		 * Noop which can be used to add breakpoints into code
         */
		void* breakpoint(opcode* pc, uint64_t* registers, registers_and_stack* env, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
            // std::breakpoint();
			MIZU_NEXT();
		}
#else
		;
#endif
        MIZU_REGISTER_INSTRUCTION_PROTOTYPE(breakpoint);
    }}

    MIZU_REGISTER_INSTRUCTION(debug::breakpoint);
}