#pragma once

#ifndef MIZU_NO_EXCEPTIONS
	#include <exception>
	#define MIZU_THROW(x) throw x
#else
	#include <cassert>
	#define MIZU_THROW(x) (assert((x, false)), abort())
#endif