#pragma once
#include "CoreDefines.h"
#include "Engine/ObjectPtr.h"

#include <atomic>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <memory>

namespace VeiM
{
	template<typename T>
	class Delegate;

	template<typename T>
	class EventDelegate;

	class Object;


	class CORE_API DelegateHandle {
	public:
		DelegateHandle() : id(GetNextId()) {}

		bool IsValid() const { return id != 0; }
		void Reset() { id = 0; }

		bool operator==(const DelegateHandle& other) const { return id == other.id; }
		bool operator!=(const DelegateHandle& other) const { return id != other.id; }

	private:
		static uint64 GetNextId() {
			static std::atomic<uint64> next_id{ 1 };
			return next_id++;
		}
		uint64 id;

		friend class std::hash<DelegateHandle>;
	};

	template<typename ReturnType, typename... Args>
	class CORE_API BoundFunctionBase {
	public:
		virtual ~BoundFunctionBase() = default;
		virtual bool IsValid() const = 0;
		virtual ReturnType Execute(Args... args) const = 0;
	};

	template<typename ReturnType, typename ObjectType, typename... Args>
	class BoundObjectFunction : public BoundFunctionBase<ReturnType, Args...> {
	public:
		using MethodPtr = ReturnType(ObjectType::*)(Args...);

		BoundObjectFunction(ObjectPtr<ObjectType> obj, MethodPtr method)
			: object(obj), methodPtr(method) {
		}

		bool IsValid() const override {
			return object.IsValid();
		}

		ReturnType Execute(Args... args) const {
			if (IsValid()) {
				return (object.Get()->*methodPtr)(args...);
			}
			if constexpr (!std::is_void_v<ReturnType>) {
				return ReturnType{};
			}
		}

	private:
		ObjectPtr<ObjectType> object;
		MethodPtr methodPtr;
	};

	template<typename ReturnType, typename ObjectType, typename... Args>
	class BoundConstObjectFunction : public BoundFunctionBase<ReturnType, Args...> {
	public:
		using MethodPtr = ReturnType(ObjectType::*)(Args...) const;

		BoundConstObjectFunction(ObjectPtr<ObjectType> obj, MethodPtr method)
			: object(obj), methodPtr(method) {
		}

		bool IsValid() const override {
			return object.IsValid();
		}

		ReturnType Execute(Args... args) const override {
			if (IsValid()) {
				return (object.Get()->*methodPtr)(args...);
			}
			if constexpr (!std::is_void_v<ReturnType>) {
				return ReturnType{};
			}
		}

	private:
		ObjectPtr<ObjectType> object;
		MethodPtr methodPtr;
	};

	template<typename ReturnType, typename... Args>
	class BoundStaticFunction : public BoundFunctionBase<ReturnType, Args...> {
	public:
		using FunctionPtr = ReturnType(*)(Args...);

		BoundStaticFunction(FunctionPtr func) : functionPtr(func) {}

		bool IsValid() const override { return functionPtr != nullptr; }

		ReturnType Execute(Args... args) const {
			if (IsValid()) {
				return functionPtr(args...);
			}
			if constexpr (!std::is_void_v<ReturnType>) {
				return ReturnType{};
			}
		}

	private:
		FunctionPtr functionPtr;
	};

	template<typename ReturnType, typename Callable, typename... Args>
	class BoundLambdaFunction : public BoundFunctionBase<ReturnType, Args...> {
	public:
		BoundLambdaFunction(Callable&& callable) : function(std::forward<Callable>(callable)) {}

		bool IsValid() const override { return true; }

		ReturnType Execute(Args... args) const {
			return function(args...);
		}

	private:
		std::decay_t<Callable> function;
	};

	template<typename ReturnType, typename... Args>
	class BoundStdFunction : public BoundFunctionBase<ReturnType, Args...> {
	public:
		BoundStdFunction(std::function<ReturnType(Args...)> func) : function(std::move(func)) {}

		bool IsValid() const override { return static_cast<bool>(function); }

		ReturnType Execute(Args... args) const override {
			if (IsValid()) {
				return function(args...);
			}
			if constexpr (!std::is_void_v<ReturnType>) {
				return ReturnType{};
			}
		}

	private:
		std::function<ReturnType(Args...)> function;
	};

	template<typename ReturnType, typename... Args>
	class Delegate<ReturnType(Args...)> {
	public:
		Delegate() = default;

		// Bind static function
		template<ReturnType(*Function)(Args...)>
		void BindStatic() {
			boundFunction = MakeShared<BoundStaticFunction<ReturnType, Args...>>(Function);
		}

		// Bind static function by pointer
		void BindStatic(ReturnType(*Function)(Args...)) {
			boundFunction = MakeShared<BoundStaticFunction<ReturnType, Args...>>(Function);
		}

		// Bind Object member function with automatic ObjectPtr management (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...)>
		void BindObject(ObjectPtr<ObjectType> object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundObjectFunction<ReturnType, ObjectType, Args...>>(object, Method);
		}

		// Bind const Object member function (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...) const>
		void BindObject(ObjectPtr<ObjectType> object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundConstObjectFunction<ReturnType, ObjectType, Args...>>(object, Method);
		}

		// Bind raw Object member function (creates ObjectPtr internally) (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...)>
		void BindObject(ObjectType* object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundObjectFunction<ReturnType, ObjectType, Args...>>(
				ObjectPtr<ObjectType>(object), Method);
		}

		// Bind const Object member function with raw pointer (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...) const>
		void BindObject(ObjectType* object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundConstObjectFunction<ReturnType, ObjectType, Args...>>(
				ObjectPtr<ObjectType>(object), Method);
		}

		// Bind Object member function with ObjectPtr (runtime version)
		template<typename ObjectType>
		void BindObject(ObjectPtr<ObjectType> object, ReturnType(ObjectType::* method)(Args...)) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundObjectFunction<ReturnType, ObjectType, Args...>>(object, method);
		}

		// Bind const Object member function with ObjectPtr (runtime version)
		template<typename ObjectType>
		void BindObject(ObjectPtr<ObjectType> object, ReturnType(ObjectType::* method)(Args...) const) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundConstObjectFunction<ReturnType, ObjectType, Args...>>(object, method);
		}

		// Bind Object member function with raw pointer (runtime version)
		template<typename ObjectType>
		void BindObject(ObjectType* object, ReturnType(ObjectType::* method)(Args...)) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundObjectFunction<ReturnType, ObjectType, Args...>>(
				ObjectPtr<ObjectType>(object), method);
		}

		// Bind const Object member function with raw pointer (runtime version)
		template<typename ObjectType>
		void BindObject(ObjectType* object, ReturnType(ObjectType::* method)(Args...) const) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			boundFunction = MakeShared<BoundConstObjectFunction<ReturnType, ObjectType, Args...>>(
				ObjectPtr<ObjectType>(object), method);
		}

		// Bind lambda or callable
		template<typename Callable>
		void BindLambda(Callable&& callable) {
			if constexpr (std::is_same_v<std::decay_t<Callable>, std::function<ReturnType(Args...)>>) {
				boundFunction = MakeShared<BoundStdFunction<ReturnType, Args...>>(std::forward<Callable>(callable));
			}
			else {
				boundFunction = MakeShared<BoundLambdaFunction<ReturnType, Callable, Args...>>(
					std::forward<Callable>(callable));
			}
		}

		// Bind std::function
		void BindFunction(std::function<ReturnType(Args...)> func) {
			boundFunction = MakeShared<BoundStdFunction<ReturnType, Args...>>(std::move(func));
		}

		// Execute the delegate
		ReturnType Execute(Args... args) const {
			if (boundFunction && boundFunction->IsValid()) {
				return boundFunction->Execute(args...);
			}
			if constexpr (!std::is_void_v<ReturnType>) {
				return ReturnType{};
			}
		}

		ReturnType operator()(Args... args) const {
			return Execute(args...);
		}

		// Check if bound and valid
		bool IsBound() const {
			return boundFunction && boundFunction->IsValid();
		}

		// Unbind
		void Unbind() {
			boundFunction.reset();
		}

		Delegate(Delegate&& other) noexcept : boundFunction(std::move(other.boundFunction)) {}
		Delegate& operator=(Delegate&& other) noexcept {
			if (this != &other) {
				boundFunction = std::move(other.boundFunction);
			}
			return *this;
		}

		
		Delegate(const Delegate& other) : boundFunction(other.boundFunction){}
		Delegate& operator=(const Delegate& other)
		{
			if (this != &other)
			{
				boundFunction = other.boundFunction;
			}
			return *this;
		}

	private:
		SharedPtr<BoundFunctionBase<ReturnType, Args...>> boundFunction;
	};


	template<typename ReturnType, typename... Args>
	class EventDelegate<ReturnType(Args...)> {
	private:
		struct BoundEntry {
			SharedPtr<BoundFunctionBase<ReturnType, Args...>> boundFunction;
			DelegateHandle handle;

			BoundEntry(SharedPtr<BoundFunctionBase<ReturnType, Args...>> func, DelegateHandle h)
				: boundFunction(std::move(func)), handle(h) {
			}

			BoundEntry(BoundEntry&& other) noexcept
				: boundFunction(std::move(other.boundFunction)), handle(other.handle) {
			}

			BoundEntry& operator=(BoundEntry&& other) noexcept {
				if (this != &other) {
					boundFunction = std::move(other.boundFunction);
					handle = other.handle;
				}
				return *this;
			}

			BoundEntry(const BoundEntry& other) : boundFunction(other.boundFunction), handle(other.handle) {}
			BoundEntry& operator=(const BoundEntry& other)
			{
				if (this != &other)
				{
					boundFunction = other.boundFunction;
					handle = other.handle;
				}
				return *this;
			}
		};

	public:
		EventDelegate() = default;

		EventDelegate(EventDelegate&& other) noexcept : functions(std::move(other.functions)) {}

		EventDelegate& operator=(EventDelegate&& other) noexcept {
			if (this != &other) {
				functions = std::move(other.functions);
			}
			return *this;
		}

		EventDelegate(const EventDelegate& other) : functions(other.functions) {}
		EventDelegate& operator=(const EventDelegate& other)
		{
			if (this != &other)
			{
				functions = other.functions;
			}
			return *this;
		}

		// Bind static function
		template<ReturnType(*Function)(Args...)>
		DelegateHandle AddStatic() {
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundStaticFunction<ReturnType, Args...>>(Function),
				handle
			);
			return handle;
		}

		// Bind static function by pointer
		DelegateHandle AddStatic(ReturnType(*Function)(Args...)) {
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundStaticFunction<ReturnType, Args...>>(Function),
				handle
			);
			return handle;
		}

		// Bind Object member function with ObjectPtr (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...)>
		DelegateHandle AddObject(ObjectPtr<ObjectType> object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundObjectFunction<ReturnType, ObjectType, Args...>>(object, Method),
				handle
			);
			return handle;
		}

		// Bind const Object member function with ObjectPtr (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...) const>
		DelegateHandle AddObject(ObjectPtr<ObjectType> object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundConstObjectFunction<ReturnType, ObjectType, Args...>>(object, Method),
				handle
			);
			return handle;
		}

		// Bind Object member function with raw pointer (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...)>
		DelegateHandle AddObject(ObjectType* object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			return AddObject<ObjectType, Method>(ObjectPtr<ObjectType>(object));
		}

		// Bind const Object member function with raw pointer (template version)
		template<typename ObjectType, ReturnType(ObjectType::* Method)(Args...) const>
		DelegateHandle AddObject(ObjectType* object) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			return AddObject<ObjectType, Method>(ObjectPtr<ObjectType>(object));
		}

		// Bind Object member function with ObjectPtr (runtime version)
		template<typename ObjectType>
		DelegateHandle AddObject(ObjectPtr<ObjectType> object, ReturnType(ObjectType::* method)(Args...)) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundObjectFunction<ReturnType, ObjectType, Args...>>(object, method),
				handle
			);
			return handle;
		}

		// Bind const Object member function with ObjectPtr (runtime version)
		template<typename ObjectType>
		DelegateHandle AddObject(ObjectPtr<ObjectType> object, ReturnType(ObjectType::* method)(Args...) const) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundConstObjectFunction<ReturnType, ObjectType, Args...>>(object, method),
				handle
			);
			return handle;
		}

		// Bind Object member function with raw pointer (runtime version)
		template<typename ObjectType>
		DelegateHandle AddObject(ObjectType* object, ReturnType(ObjectType::* method)(Args...)) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			return AddObject(ObjectPtr<ObjectType>(object), method);
		}

		// Bind const Object member function with raw pointer (runtime version)
		template<typename ObjectType>
		DelegateHandle AddObject(ObjectType* object, ReturnType(ObjectType::* method)(Args...) const) {
			static_assert(std::is_base_of_v<Object, ObjectType>, "ObjectType must derive from Object");
			return AddObject(ObjectPtr<ObjectType>(object), method);
		}

		// Bind lambda or callable
		template<typename Callable>
		DelegateHandle AddLambda(Callable&& callable) {
			auto handle = DelegateHandle{};
			if constexpr (std::is_same_v<std::decay_t<Callable>, std::function<ReturnType(Args...)>>) {
				functions.emplace_back(
					MakeShared<BoundStdFunction<ReturnType, Args...>>(std::forward<Callable>(callable)),
					handle
				);
			}
			else {
				functions.emplace_back(
					MakeShared<BoundLambdaFunction<ReturnType, Callable, Args...>>(
						std::forward<Callable>(callable)),
					handle
				);
			}
			return handle;
		}

		// Bind std::function
		DelegateHandle AddFunction(std::function<ReturnType(Args...)> func) {
			auto handle = DelegateHandle{};
			functions.emplace_back(
				MakeShared<BoundStdFunction<ReturnType, Args...>>(std::move(func)),
				handle
			);
			return handle;
		}

		// Remove specific binding
		bool Remove(const DelegateHandle& handle) {
			auto it = std::find_if(functions.begin(), functions.end(),
				[&handle](const BoundEntry& entry) {
					return entry.handle == handle;
				});

			if (it != functions.end()) {
				functions.erase(it);
				return true;
			}
			return false;
		}

		// Clean up invalid object references
		void Cleanup() {
			functions.erase(
				std::remove_if(functions.begin(), functions.end(),
					[](const BoundEntry& entry) {
						return !entry.boundFunction->IsValid();
					}),
				functions.end()
			);
		}

		// Remove all bindings
		void Clear() {
			functions.clear();
		}

		// Execute all bound functions
		void Broadcast(Args... args) {
			// Clean up invalid references first
			Cleanup();

			for (const auto& entry : functions) {
				if (entry.boundFunction->IsValid()) {
					entry.boundFunction->Execute(args...);
				}
			}
		}

		void operator()(Args... args) {
			Broadcast(args...);
		}

		// Check if any functions are bound
		bool IsBound() const {
			return !functions.empty();
		}

		// Get number of bound functions (including potentially invalid ones)
		size_t GetBoundCount() const {
			return functions.size();
		}

		// Get number of valid bound functions
		size_t GetValidBoundCount() const {
			return std::count_if(functions.begin(), functions.end(),
				[](const BoundEntry& entry) {
					return entry.boundFunction->IsValid();
				});
		}

	private:
		std::vector<BoundEntry> functions;
	};

}

#define DECLARE_DELEGATE(DelegateName) \
    using DelegateName = VeiM::Delegate<void()>

#define DECLARE_DELEGATE_1p(DelegateName, Param1Type) \
    using DelegateName = VeiM::Delegate<void(Param1Type)>

#define DECLARE_DELEGATE_2p(DelegateName, Param1Type, Param2Type) \
    using DelegateName = VeiM::Delegate<void(Param1Type, Param2Type)>

#define DECLARE_DELEGATE_3p(DelegateName, Param1Type, Param2Type, Param3Type) \
    using DelegateName = VeiM::Delegate<void(Param1Type, Param2Type, Param3Type)>

#define DECLARE_DELEGATE_4p(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type) \
    using DelegateName = VeiM::Delegate<void(Param1Type, Param2Type, Param3Type, Param4Type)>

#define DECLARE_DELEGATE_5p(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
    using DelegateName = VeiM::Delegate<void(Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)>


#define DECLARE_DELEGATE_r(ReturnType, DelegateName) \
    using DelegateName = VeiM::Delegate<ReturnType()>

#define DECLARE_DELEGATE_r1p(ReturnType, DelegateName, Param1Type) \
    using DelegateName = VeiM::Delegate<ReturnType(Param1Type)>

#define DECLARE_DELEGATE_r2p(ReturnType, DelegateName, Param1Type, Param2Type) \
    using DelegateName = VeiM::Delegate<ReturnType(Param1Type, Param2Type)>

#define DECLARE_DELEGATE_r3p(ReturnType, DelegateName, Param1Type, Param2Type, Param3Type) \
    using DelegateName = VeiM::Delegate<ReturnType(Param1Type, Param2Type, Param3Type)>

#define DECLARE_DELEGATE_r4p(ReturnType, DelegateName, Param1Type, Param2Type, Param3Type, Param4Type) \
    using DelegateName = VeiM::Delegate<ReturnType(Param1Type, Param2Type, Param3Type, Param4Type)>

#define DECLARE_DELEGATE_r5p(ReturnType, DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
    using DelegateName = VeiM::Delegate<ReturnType(Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)>

// Multicast delegates
#define DECLARE_EVENT(DelegateName) \
    using DelegateName = VeiM::EventDelegate<void()>

#define DECLARE_EVENT_1p(DelegateName, Param1Type) \
    using DelegateName = VeiM::EventDelegate<void(Param1Type)>

#define DECLARE_EVENT_2p(DelegateName, Param1Type, Param2Type) \
    using DelegateName = VeiM::EventDelegate<void(Param1Type, Param2Type)>

#define DECLARE_EVENT_3p(DelegateName, Param1Type, Param2Type, Param3Type) \
    using DelegateName = VeiM::EventDelegate<void(Param1Type, Param2Type, Param3Type)>

#define DECLARE_EVENT_4p(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type) \
    using DelegateName = VeiM::EventDelegate<void(Param1Type, Param2Type, Param3Type, Param4Type)>

#define DECLARE_EVENT_5p(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
    using DelegateName = VeiM::EventDelegate<void(Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)>

namespace std {
	template<>
	struct hash<VeiM::DelegateHandle> {
		size_t operator()(const VeiM::DelegateHandle& handle) const {
			return hash<uint64_t>()(handle.id);
		}
	};
}