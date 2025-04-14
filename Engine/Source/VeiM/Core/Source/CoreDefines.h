#pragma once
#include "Misc/Build.h"


#ifdef VM_DEBUG
	#define VM_DEBUGBREAK() __debugbreak()
	#define VM_ENABLE_ASSERTS
	#define VM_ENABLE_PROFILING
	#define VM_LOG_LEVEL_DEBUG
#else
	#define VM_DEBUGBREAK()
#endif


#define VM_EXPAND_MACRO(x) x
#define VM_STRINGIFY_MACRO(x) #x
#define BIT(x) (1 << x)
#define TEXT(quote) L##quote

#define VM_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) {return this->fn(std::forward<decltype(args)>(args)...);}


#ifdef VM_ENABLE_ASSERTS
	#define VM_INTERNAL_ASSERT_IMPL(type, check, msg, ...) {if(!(check)) { VM##type##ERROR(msg, __VA_ARGS__); VM_DEBUGBREAK();}}
	#define VM_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VM_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}: at {1}:{2}", __VA_ARGS__, std::filesystem::path(__FILE__).filename().filename().string(), __LINE__)
	#define	VM_INTERNAL_ASSERT_NO_MSG(type, check) VM_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VM_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().filename().string(), __LINE__)
	
	#define VM_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define VM_INTERNAL_ASSERT_GET_MACRO(...) VM_EXPAND_MACRO(VM_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VM_INTERNAL_ASSERT_WITH_MSG, VM_INTERNAL_ASSERT_NO_MSG))
	
	#define	VM_ASSERT(...) VM_EXPAND_MACRO( VM_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
	#define	VM_CORE_ASSERT(...) VM_EXPAND_MACRO( VM_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
	#define VM_ASSERT(...) // TODO: use VM_ERROR
	#define VM_CORE_ASSERT(...) // TODO: use VM_CORE_ERROR
#endif

#if IS_UNIFIED
	#define CORE_API
#else
	#ifdef _WIN32
		#ifdef CORE_EXPORTS
			#define CORE_API __declspec(dllexport)
		#else
			#define CORE_API __declspec(dllimport)
		#endif
	#else
		#define CORE_API
	#endif
#endif


#if defined(_MSC_VER) 
	#define FORCEINLINE __forceinline
#else 
	#define FORCEINLINE inline
#endif

#include "Misc/Misc.h"


#include <memory>
#include <string>
#include <filesystem>
#include <cstdint>
#include <tchar.h>

namespace VeiM
{
	using byte = uint8_t;
	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;

	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;
	using size_t = ::std::size_t;

	using String = ::std::string;
	namespace fs = std::filesystem;

   template<typename T>
   using UniquePtr = std::unique_ptr<T>;
   template<typename T, typename ... Args>
   constexpr UniquePtr<T> MakeUnique(Args&& ... args)
   {
	   return std::make_unique<T>(std::forward<Args>(args)...);
   }

   template<typename T>
   using SharedPtr = std::shared_ptr<T>;
   template<typename T, typename ... Args>
   constexpr SharedPtr<T> MakeShared(Args&& ... args)
   {
	   return std::make_shared<T>(std::forward<Args>(args)...);
   }
}

#include "Logging/Log.h"
