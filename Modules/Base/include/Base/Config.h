#pragma once

#if defined(CMAKE_DEBUG_MACRO)
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

#if defined(_MSC_VER)
	#define ENGINE_COMPILER_MSVC
#elif defined(__clang__)
	#define ENGINE_COMPILER_CLANG
#elif defined(__GNUC__)
	#define ENGINE_COMPILER_GCC
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define ENGINE_PLATFORM_WINDOWS
#else
	#define ENGINE_PLATFORM_LINUX
#endif

#if defined(ENGINE_COMPILER_MSVC)
	#define ENGINE_FORCEINLINE __forceinline
#elif defined(ENGINE_COMPILER_CLANG) || defined(ENGINE_COMPILER_GCC)
	#define ENGINE_FORCEINLINE __attribute__((always_inline)) inline
#else
	#define ENGINE_FORCEINLINE inline
#endif

#if defined(ENGINE_PLATFORM_WINDOWS)
	#define ENGINE_EXPORT __declspec(dllexport)
	#define ENGINE_IMPORT __declspec(dllimport)
#else // Linux / macOS / Unix
	#if defined(ENGINE_COMPILER_GCC) || defined(ENGINE_COMPILER_CLANG)
		#define ENGINE_EXPORT __attribute__((visibility("default")))
		#define ENGINE_IMPORT
	#else
		#define ENGINE_EXPORT
		#define ENGINE_IMPORT
	#endif
#endif