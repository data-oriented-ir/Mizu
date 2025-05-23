![Mizu's Mascot](https://github.com/joshuadahlunr/MizuVM/blob/main/docs/mizu.svg)

# Mizu

[![Unlicensed](https://flat.badgen.net/github/license/joshuadahlunr/mizuvm)](https://github.com/joshuadahlunr/mizuvm/blob/main/LICENSE) 

Mizu is a lightweight easily extensible interpreter loosely modeled off of RISC-V assembly.

Programs are represented as an array of opcodes which are then allowed to manipulate virtual registers and a virtual stack.
The simplest "useful" Mizu program is:

```cpp
#define MIZU_IMPLEMENTATION
#include <mizu/instructions.hpp>

const static mizu::opcode program[] = {
    mizu::opcode{mizu::load_immediate, mizu::registers::t(0)}.set_immediate(40),
    mizu::opcode{mizu::debug_print, 0, mizu::registers::t(0)},
    mizu::opcode{mizu::halt},
};

int main() {
    mizu::registers_and_stack environment = {};
    setup_environment(environment);

    MIZU_START_FROM_ENVIRONMENT(program, environment);
}
```

Take a read through the [documentorial](https://joshuadahlunr.github.io/MizuVM/) (documentation and tutorial rolled into one) to learn about the details of Mizu programs and our instructions.

## Features

* Small (our test executables are less than 50kb)
* Portable (should run anywhere with a C++ compiler[^1], even microcontrollers and the web)
* Fast (recursively calculates the 40th Fibonacci a full second faster than Lua)
* Header-only[^2]/Easily embeddable as a library!
* Simple and easily extensible instructions
* Integer and float support
* Built in threading instructions (also includes simple go-like channels for inter-thread communication)
* Fallback coroutine "threading" for single core machines
* Built in FFI instructions (Mizu can call external functions in the same way it calls its own, just substitute the jump_to instruction for ffi::call)
* Portable Binary Format and a [runner for it](https://github.com/joshuadahlunr/mizurunner).

[^1] Our architecture requires tail call optimization, compilers which don't support that (avr-gcc, MSVC) will compile but can only run the simplest of programs!

[^2] FFI support relies on [libffi](https://github.com/libffi/libffi) which is not header-only!

## Tail Call Warning

Mizu makes heavy use of tail call optimization, if this optimization is not present programs will likely crash.
At the moment only clang has been configured to support tail call operations in debug mode.
**Visual Studio will likely fail!** Testing has shown the Microsoft's Visual Studio compiler doesn't tail call optimize, thus on Windows using Msys or Mingw is necessary!

## How to Integrate

The easiest way to integrate Mizu is to add it as a sub_directory using CMake:

```cmake
add_subdirectory(path/to/mizu)
...
target_link_libraries(your_cmake_target PUBLIC mizu::vm)
```

Then just include `mizu/instructions.hpp` and define `MIZU_IMPLEMENTATION` in one of your source files.

### Hard Way

You can also make sure `./` and `./thirdparty/libfp` are marked as include directories. Then if you are going to use FFI make sure libffi also gets built, linked, and included.
