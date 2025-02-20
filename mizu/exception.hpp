#pragma once

#include <exception>

#ifndef MIZU_NO_EXCEPTIONS
    #define MIZU_THROW(x) throw x
#else
    #define MIZU_THROW(x) (assert((x, false)), abort())
#endif