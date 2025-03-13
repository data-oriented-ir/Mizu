.. a documentation master file, created by
	sphinx-quickstart on Sun Mar  3 23:49:53 2019.
	You can adapt this file completely to your liking, but it should at least
	contain the root `toctree` directive.

Welcome to Mizu's Documentorial!
================================

.. toctree::
	:maxdepth: 2
	:caption: Contents:

This documentation page is designed less like a direct reference and more like a tutorial.
If you would instead prefer a reference... :doc:`full generated documentation can be found here<doxygen>`.

Programs in Mizu are represented by an array of Opcodes.

.. doxygenstruct:: mizu::opcode
	:project: mizu_doxygen
	:members:

.. doxygentypedef:: mizu::instruction_t
	:project: mizu_doxygen

A simple program that loads a number and prints it out might look something like:

.. code:: c++

	#define MIZU_IMPLEMENTATION // Needs to be included in exactly one source (.cpp) file
	#include <mizu/instructions.hpp>

	const static opcode program[] = { // Const static so that the code winds up in .bss
		opcode{load_immediate, registers::t(0)}.set_immediate(40),
		opcode{debug_print, 0, registers::t(0)},
		opcode{halt}, // Need to explicitly mark the end of execution
	};

This code looks up t0 (first temporary) register using Mizu's register utilities.

.. doxygennamespace:: mizu::registers
	:project: mizu_doxygen

Unfortunately these "registers" must be stored in memory, thus before mizu can run a program it must allocate memory for the registers (and the stack).
This memory is cloned for each thread of execution.

.. note:: 

	All Mizu registers are unsigned 64 bit integers (u64) regardless of host machine.

.. doxygenstruct:: mizu::registers_and_stack
	:project: mizu_doxygen
	:members:

.. doxygenfunction:: mizu::setup_environment
	:project: mizu_doxygen

Once an environment has been setup a program can be started in that environment using:

.. doxygendefine:: MIZU_START_FROM_ENVIRONMENT
	:project: mizu_doxygen

The following code provides a demonstration of the program initialization process:

.. code:: c++
	
	{
		registers_and_stack environment = {};
		setup_environment(environment); // Ensures the pointers within the environment are properly configured

		MIZU_START_FROM_ENVIRONMENT(program, env);
	}

Serialization
-------------

Once a program has been created it may be useful to save it to a file. 
Since different programs will wind up putting the instructions' functions in different places in memory, when serializing a file it is useful to instead replace those addresses with indices.
Thus Mizu provides a lookup system:

.. doxygengroup:: lookup
	:project: mizu_doxygen

As well as some serialization functions which replace pointers with their identifiers automatically:

.. doxygenfile:: serialize.hpp
	:project: mizu_doxygen	

Instructions
----------

New instructions (almost) always follow this template:

.. code:: c++
	
	void* <name> (opcode* pc, uint64_t* registers, uint8_t* stack_boundary, uint8_t* sp) {
		// Instruction code goes here
		MIZU_NEXT();
	}
	MIZU_REGISTER_INSTRUCTION(<name>); // Ensures the serialization system is aware of the instruction

The program counter (pc) variable points to the current opcode and provides its out, a, and p parameters which can be used to lookup the relevant register in the registers variable.
The stack boundary marks where the stack should stop and the registers begin, while stack pointer (sp) variable marks the current top of the stack.
The code to implement the instruction should be written before the MIZU_NEXT macro which is responsible for handling the flow of control between instructions.

The parameters for the following instructions are described in terms of their opcodes:
	- `out`, `a`, and `b` all represent the relevant members of an opcode
	- `b` can sometimes take an explicitly signed value which can be set using the .set_branch_immediate() function.
	- `immediate` represents a value taking the space of both a and b set using the .set_immediate() function.
	- `signed immediate` represents a value taking the space of both a and b set using the .set_signed_immediate() function.
	- `float immediate` represents a floating point value taking the space of both a and b set using .set_immediate_f32().

.. doxygenfile:: core.hpp
	:project: mizu_doxygen

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`