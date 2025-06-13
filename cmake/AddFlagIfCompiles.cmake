# From: https://github.com/raysan5/raylib/blob/master/cmake/AddIfFlagCompiles.cmake

include(CheckCCompilerFlag)
function(add_if_flag_compiles flag)
  CHECK_C_COMPILER_FLAG("${flag}" COMPILER_HAS_THOSE_TOGGLES)
  set(outcome "Failed")
  if(COMPILER_HAS_THOSE_TOGGLES)
    foreach(var ${ARGN})
      set(${var} "${flag} ${${var}}" PARENT_SCOPE)
    endforeach()
    set(outcome "compiles")
  endif()
  message(STATUS "Testing if ${flag} can be used -- ${outcome}")
endfunction()