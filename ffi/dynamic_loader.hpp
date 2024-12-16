#pragma once

#if defined(__linux__) || defined(__APPLE__)
	#include <dlfcn.h>
#endif
#ifdef _WIN32
	#include <windows.h>
#endif

#if defined(__EMSCRIPTEN__) || defined(__WASI__) || defined(__wasm__)
	#define MIZU_WASM
#endif

#include <stdexcept>
#include <string>
#include "../exception.hpp"

namespace mizu::loader {
	struct error: public std::runtime_error { using std::runtime_error::runtime_error; };
	struct library;

	inline library* load_shared(std::string_view path, bool append_platform_decorator = false) {
		std::string decorated = std::string{ path };
#if defined(__linux__) || defined(__APPLE__)
	#ifdef __linux__
		if(append_platform_decorator) decorated += ".so";
	#elif defined(__APPLE__)
		if(append_platform_decorator) decorated += ".dylib";
	#endif
		auto out = dlopen(decorated.c_str(), RTLD_LAZY);
		if(!out) MIZU_THROW(error(dlerror()));
		return (library*)out;
#elif defined(_WIN32)
		if (append_platform_decorator) decorated += ".dll";
		auto out = LoadLibraryA(decorated.c_str());
		if(!out) MIZU_THROW(error("Failed to load library"));
		return (library*)out;
#else
		// static_assert(false, "Dynamic loader is not supported on this platform!");
#endif
	}
	inline library* load_dynamic(std::string_view path, bool append_platform_decorator = false) {
		return load_shared(path, append_platform_decorator);
	}
	inline library* load_library(std::string_view path, bool append_platform_decorator = false) {
		return load_shared(path, append_platform_decorator);
	}

	inline library* load_current_executable() {
#if defined(__linux__) || defined(__APPLE__)
		auto out = dlopen(nullptr, RTLD_LAZY);
		if(!out) MIZU_THROW(error(dlerror()));
		return (library*)out;
#elif defined(_WIN32)
		return load_library(MIZU_LOAD_CURRENT_EXECUTABLE_PATH, false);
#endif
	}

	inline void* lookup(std::string_view name, library* lib = nullptr) {
#if defined(__linux__) || defined(__APPLE__)
		auto out = dlsym(lib, name.data());
		if(auto err = dlerror(); err) MIZU_THROW(error(err));
		return out;
#elif defined(_WIN32)
		if(lib == nullptr) lib = load_current_executable();
		auto out = GetProcAddress((HMODULE)lib, name.data()); 
		if(!out) MIZU_THROW(error("Failed to find function"));
		return out;
#else
		// static_assert(false, "Dynamic loader is not supported on this platform!");
#endif
	}

	inline void close(library* lib) {
#if defined(__linux__) || defined(__APPLE__)
		dlclose(lib);
		if(auto err = dlerror(); err) MIZU_THROW(error(err));
#elif defined(_WIN32)
		if(!FreeLibrary((HMODULE)lib)) MIZU_THROW(error("Failed to close library"));
#else
		// static_assert(false, "Dynamic loader is not supported on this platform!");
#endif
	} 
}