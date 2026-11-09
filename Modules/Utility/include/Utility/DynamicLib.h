#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

namespace Engine::Util {
    struct LibraryHandle {
        void* handle;
    };

    class DynamicLibLoader {
    public:
        void* load_library(const char* lib_path) {
#if defined(_WIN32) || defined(_WIN64)
            return (void*)LoadLibraryA(lib_path);
#else
            return (void*)dlopen(lib_path, RTLD_NOW | RTLD_LOCAL);
#endif
        }

        void* get(void* handle, const char* name) {
#if defined(_WIN32) || defined(_WIN64)
            return (void*)GetProcAddress((HMODULE)handle, name);
#else
            return dlsym(handle, name);
#endif
        }

        void unload_library(void* handle) {
#if defined(_WIN32) || defined(_WIN64)
            if (handle) FreeLibrary((HMODULE)handle);
#else
            if (handle) dlclose(handle);
#endif
        }
    };
}