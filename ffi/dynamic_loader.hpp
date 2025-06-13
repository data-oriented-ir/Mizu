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
#include <fp/pointer.hpp>
#include "../mizu/exception.hpp"

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

	inline library* load_first_that_exists(fp::view<const std::string_view> paths, bool append_platform_decorator = false) {
		for(auto path: paths) {
#ifndef MIZU_NO_EXCEPTIONS
			try {
#endif
				auto lib = load_shared(path, append_platform_decorator);
				if(lib) return lib;
#ifndef MIZU_NO_EXCEPTIONS
			} catch(error) { /* do nothing */}
#endif
		}
		MIZU_THROW(error("Failed to load any of the provided libraries!"));
	}

	inline library* load_current_executable() {
#if defined(__linux__) || defined(__APPLE__)
		auto out = dlopen(nullptr, RTLD_LAZY);
		if(!out) MIZU_THROW(error(dlerror()));
		return (library*)out;
#elif defined(_WIN32)
	#ifdef MIZU_LOAD_CURRENT_EXECUTABLE_PATH
		return load_library(MIZU_LOAD_CURRENT_EXECUTABLE_PATH, false);
	#else
		MIZU_THROW("In order to load the current executable on windows the app must be setup as a dynamic executable with the add_dynamic_executable CMake function!");
		return nullptr;
	#endif
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
		if(!out) MIZU_THROW(error("Failed to find function: " + std::string(name)));
		return (void*)out;
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