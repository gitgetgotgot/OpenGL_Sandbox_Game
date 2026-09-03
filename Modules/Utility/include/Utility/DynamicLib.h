#pragma once

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

#include <filesystem>

class DynamicLibLoader {
public:
    bool load_library(const char* lib_path) {
#if defined(_WIN32)
        handle = LoadLibraryA(lib_path);
#else
        handle = dlopen(lib_path, RTLD_NOW);
#endif
        return handle != nullptr;
    }

    void* get(const char* name) {
#if defined(_WIN32)
        return (void*)GetProcAddress((HMODULE)handle, name);
#else
        return dlsym(handle, name);
#endif
    }

    void unload_library() {
#if defined(_WIN32)
        if (handle) FreeLibrary((HMODULE)handle);
#else
        if (handle) dlclose(handle);
#endif
    }

private:
    void* handle = nullptr;
};