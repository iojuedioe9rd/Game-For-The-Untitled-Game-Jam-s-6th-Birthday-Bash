#pragma once
#include <filesystem>

typedef unsigned int ALuint;

namespace std
{
	namespace fs = filesystem;
}

#include "PlatformDetection.h"
#include <memory>




#define BIT(x) (1 << x)
#define NULL 0
#ifndef NDEBUG
#define ENGINE_ENABLE_ASSERTS 1
#endif // !VX_DIST

#define ENGINE_STD_PATH_CSTR(path) (path.string().c_str())

#ifdef ENGINE_ENABLE_ASSERTS
	#if defined(ENGINE_PLATFORM_WINDOWS)
		#define ENGINE_DEBUGBREAK() __debugbreak()
	#elif defined(ENGINE_PLATFORM_LINUX)
		#include <signal.h>
		#define ENGINE_DEBUGBREAK() raise(SIGTRAP)
	#elif defined(ENGINE_PLATFORM_WEBGL)
		extern "C" void EM_DEBUGBREAK(const char* file, int line);
		
		#define ENGINE_DEBUGBREAK() EM_DEBUGBREAK(__FILE__, __LINE__)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define ENGINE_ENABLE_ASSERTS
#else
	#define ENGINE_DEBUGBREAK()
#endif



#define ENGINE_EXPAND_MACRO(x) x
#define ENGINE_STRINGIFY_MACRO(x) #x

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }



namespace Engine {

	template<typename T>
	struct ENGINE_API SafeObject
	{
		bool isGood = false;
		T object;

		SafeObject() : isGood(false), object() {}
		SafeObject(T obj) : isGood(true), object(obj) {}

		operator bool() const { return isGood; }
		operator T() { return object; }
		operator const T() const { return object; }
		operator T& () { return object; }

		T& get() { return object; }
		const T& get() const { return object; }
	};
}


namespace Engine {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

