#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"

#include "Engine/ObjectPtr.h"
#include "Engine/WeakObjectPtr.h"
#include "Engine/SoftObjectPtr.h"

#include <functional>
#include <typeindex>
#include <type_traits>
#include <map>


template <typename T>
concept HasGetMethod = requires(T t) { t.Get(); };

// Reflection.h
namespace VeiM
{


	template <typename T>
	struct is_object_ptr : std::false_type {};
	template <typename U>
	struct is_object_ptr<ObjectPtr<U>> : std::true_type {};
	template <typename T>
	concept ObjectPtrType = is_object_ptr<std::remove_cvref_t<T>>::value;

	//  Add Serialization support for property descriptor, but not yet

	class Object;
	class GarbageCollector;


	template<typename T>
	struct ContainerHandler;
#if 0

	// Concept of metadata for ediotr integration
	struct PropertyMetaData {
		StringID DisplayName;
		StringID Category;
		float MinValue = 0.f;
		float MaxValue = 0.f;
		bool bVisible = true;
		bool bEditable = true;
	};

#endif


	//  Add to PropertyDescriptor, but not yet
	// 	template<typename T>
	// 	static PropertyType DeducePropertyType() {
	// 		if constexpr (std::is_same_v<T, bool>) return PropertyType::Bool;
	// 		else if constexpr (std::is_integral_v<T>) return PropertyType::Int;
	// 		else if constexpr (std::is_floating_point_v<T>) return PropertyType::Float;
	// 		// ... other type mappings
	// 	}

	struct CORE_API PropertyDescriptor
	{
		StringID Name;
		EStringID Type;			// Property type (basic, object, vector, map)
		EStringID ElementType;	// Element type for containers (Int, Float, ObjectProperty, etc.)
		EStringID KeyType;		// Key type for maps
		size_t Offset;
		size_t Size;
		bool bIsContainer;
		std::type_index TypeIndex;
		std::type_index ElementTypeIndex; // Type index for container elements
		std::type_index KeyTypeIndex;     // Type index for map keys

		std::function<void(void*, GarbageCollector&)> MarkReferencedObjects = nullptr;

		// For containers: functions to access elements and iterate
		std::function<size_t(void*)> GetContainerSize = nullptr;
		std::function<void* (void*, size_t)> GetElementPtr = nullptr;
		std::function<void(void*, void*, void*)> MapIterator = nullptr; // Instance, KeyCallback, ValueCallback

		PropertyDescriptor()
			: TypeIndex(typeid(void))
			, ElementTypeIndex(typeid(void))
			, KeyTypeIndex(typeid(void))
			, bIsContainer(false)
		{
		}

		const char* GetTypeName() const {
			return StringID(Type).Get();
		}

		template<typename T>
		T& GetValue(void* instance) const
		{
			if (std::type_index(typeid(T)) != TypeIndex) {
				VM_ERROR("Type mismatch in property access"); // Temporary
				VM_ASSERT(false, "Type mismatch in property access");
			}

			return *reinterpret_cast<T*>(reinterpret_cast<char*>(instance) + Offset);
		}


		template<typename T>
		void SetValue(void* instance, const T& value) const
		{
			if (std::type_index(typeid(T)) != TypeIndex) {
				VM_ERROR("Type mismatch in property access"); // Temporary
				VM_ASSERT(false, "Type mismatch in property access");
			}

			*reinterpret_cast<T*>(reinterpret_cast<char*>(instance) + Offset) = value;
		}

		void CopyValue(void* source, void* destination) const
		{
			static const std::unordered_map<std::type_index, std::function<void(const PropertyDescriptor*, void*, void*)>> copyFuncs = {
	   {typeid(bool), [](const PropertyDescriptor* prop, void* src, void* dst) {
		   prop->SetValue<bool>(dst, prop->GetValue<bool>(src));
	   }},
	   {typeid(int), [](const PropertyDescriptor* prop, void* src, void* dst) {
		   prop->SetValue<int>(dst, prop->GetValue<int>(src));
	   }},
	   {typeid(float), [](const PropertyDescriptor* prop, void* src, void* dst) {
		   prop->SetValue<float>(dst, prop->GetValue<float>(src));
	   }},
	   {typeid(String), [](const PropertyDescriptor* prop, void* src, void* dst) {
		   prop->SetValue<String>(dst, prop->GetValue<String>(src));
	   }},
	   {typeid(StringID), [](const PropertyDescriptor* prop, void* src, void* dst) {
		   prop->SetValue<StringID>(dst, prop->GetValue<StringID>(src));
	   }},
				// Add more types
			};

			auto it = copyFuncs.find(TypeIndex);
			if (it != copyFuncs.end()) {
				it->second(this, source, destination);
			}
			else {
				VM_ASSERT(false, "Unsupported type in CopyValue");
			}
		}

		void CopyContainer(void* source, void* destination) const
		{
			// Implement
		}

		ObjectPtr<Object> GetAsObjectPtr(void* instance) const
		{
			if (!IsObjectPtrType())
			{
				VM_ERROR("Property is not an ObjectPtr");
				return ObjectPtr<Object>();
			}

			return *reinterpret_cast<ObjectPtr<Object>*>(
				reinterpret_cast<char*>(instance) + Offset);
		}

		bool IsObjectPtrType() const
		{
			return Type == EStringID::ObjectProperty ||
				Type == EStringID::SoftObjectProperty;
		}

		bool IsContainerType() const
		{
			return bIsContainer;
		}

		bool IsVectorType() const
		{
			return Type == EStringID::VectorProperty;
		}

		bool IsMapType() const
		{
			return Type == EStringID::MapProperty;
		}

		template<typename T>
		T& GetVectorElement(void* instance, size_t index) const
		{
			if (!IsVectorType()) {
				VM_ERROR("Property is not a vector type");
				static T dummy{};
				return dummy;
			}

			if (std::type_index(typeid(T)) != ElementTypeIndex) {
				VM_ERROR("Element type mismatch in vector access");
				static T dummy{};
				return dummy;
			}

			void* elementPtr = GetElementPtr(instance, index);
			return *reinterpret_cast<T*>(elementPtr);
		}

		size_t GetSize(void* instance) const
		{
			if (!IsContainerType() || !GetContainerSize) {
				return 0;
			}
			return GetContainerSize(instance);
		}


	};

	class CORE_API ClassDescriptor
	{
	public:
		StringID Name;
		std::unordered_map<StringID, PropertyDescriptor> Properties;
		std::function<Object* ()> ConstructorFunc;
		const ClassDescriptor* ParentClass;
#ifdef VM_WITH_EDITOR
		String DebugName;
#endif

		template<typename T>
		static ClassDescriptor* Get()
		{
			return T::StaticClass();
		}

		bool IsChildOf(const ClassDescriptor* potentialParent) const
		{
			const ClassDescriptor* current = this;
			while (current)
			{
				if (current == potentialParent)
				{
					return true;
				}
				current = current->ParentClass;
			}
			return false;
		}

		Object* CreateInstance() const
		{
			return ConstructorFunc ? ConstructorFunc() : nullptr;
		}
	};

	class CORE_API ClassRegistry
	{
		// Later add CDO
	public:
		static void InitializeReflectionSystem()
		{
			ClassRegistry::InitializeAllClasses();
		}

	private:
		static std::unordered_map<StringID, ClassDescriptor*>& GetRegistry()
		{
			static std::unordered_map<StringID, ClassDescriptor*> registry;
			return registry;
		}

		static std::vector<std::function<void()>>& GetRegistrationCallbacks() {
			static std::vector<std::function<void()>> callbacks;
			return callbacks;
		}
	public:
		template<typename T>
		static void AddRegistrationCallback() {
			GetRegistrationCallbacks().emplace_back([] {
				T::StaticClass();
				});
		}

		static void InitializeAllClasses() {
			for (auto& callback : GetRegistrationCallbacks()) {
				callback();
			}
		}
	public:
		static void RegisterClass(ClassDescriptor* classDesc)
		{
			GetRegistry()[classDesc->Name] = classDesc;
		}

		static ClassDescriptor* FindClass(StringID className)
		{
			auto& registry = GetRegistry();
			auto it = registry.find(className);
			if (it != registry.end())
			{
				return it->second;
			}
			return nullptr;
		}

		static std::unordered_map<StringID, ClassDescriptor*> GetAllClasses()
		{
			std::unordered_map<StringID, ClassDescriptor*> result = GetRegistry();
			return result;
		}
		template <typename T>
		static std::map<StringID, ClassDescriptor*, StringIDComparator> GetAllClassesOfClass()
		{
			return GetAllClassesOfClass(T::StaticClass());
		}

		static std::map<StringID, ClassDescriptor*, StringIDComparator> GetAllClassesOfClass(ClassDescriptor* baseClass)
		{
			std::unordered_map<StringID, ClassDescriptor*>& allClasses = GetRegistry();
			std::map<StringID, ClassDescriptor*, StringIDComparator> classesMap;
			for (auto& classRecord : allClasses)
			{
				if (classRecord.second->IsChildOf(baseClass))
				{
					classesMap.insert(classRecord);
				}
			}
			return classesMap;
		}
	};

	// Container handler for std::vector
	template<typename T>
	struct ContainerHandler<std::vector<T>> {
		static size_t GetSize(const std::vector<T>& container) {
			return container.size();
		}

		static T* GetElementPtr(std::vector<T>& container, size_t index) {
			if (index >= container.size()) {
				VM_ERROR("Vector index out of bounds");
				return nullptr;
			}
			return &container[index];
		}

		static void MarkReferencedObjects(std::vector<T>& container, GarbageCollector& gc)
		{
			if constexpr (ObjectPtrType<T>)
			{
				for (auto& item : container)
				{
					if (item.Get())
					{
						gc.MarkReachable(item.Get());
					}
				}
			}
		}

		static EStringID GetElementType() {
			if constexpr (std::is_same_v<T, int32_t>) return EStringID::IntProperty;
			else if constexpr (std::is_same_v<T, float>) return EStringID::FloatProperty;
			else if constexpr (std::is_same_v<T, bool>) return EStringID::BoolProperty;
			else if constexpr (std::is_same_v<T, String>) return EStringID::StringProperty;
			else if constexpr (std::is_same_v<T, ObjectPtr<Object>>) return EStringID::ObjectProperty;
			else if constexpr (std::is_same_v<T, SoftObjectPtr<Object>>) return EStringID::SoftObjectProperty;
			else return EStringID::None;
		}
	};

	// Container handler for std::unordered_map
	template<typename K, typename V>
	struct ContainerHandler<std::unordered_map<K, V>> {
		static size_t GetSize(const std::unordered_map<K, V>& container) {
			return container.size();
		}

		static void IterateMap(std::unordered_map<K, V>& container,
			void* keyCallback, void* valueCallback) {
			auto keyFunc = reinterpret_cast<void(*)(const K&)>(keyCallback);
			auto valueFunc = reinterpret_cast<void(*)(V&)>(valueCallback);

			for (auto& [key, value] : container) {
				if (keyFunc) keyFunc(key);
				if (valueFunc) valueFunc(value);
			}
		}

		static void MarkReferencedObjects(std::unordered_map<K, V>& container, GarbageCollector& gc)
		{
			if constexpr (HasGetMethod<V> && ObjectPtrType<V>)
			{
				for (auto& [key, value] : container)
				{
					if (value.Get())
					{
						gc.MarkReachable(value.Get());
					}
				}
			}
		}

		static EStringID GetKeyType() {
			if constexpr (std::is_same_v<K, int32_t>) return EStringID::IntProperty;
			else if constexpr (std::is_same_v<K, float>) return EStringID::FloatProperty;
			else if constexpr (std::is_same_v<K, bool>) return EStringID::BoolProperty;
			else if constexpr (std::is_same_v<K, String>) return EStringID::StringProperty;
			else if constexpr (std::is_same_v<K, StringID>) return EStringID::StringIDProperty;
			else return EStringID::None;
		}

		static EStringID GetValueType() {
			if constexpr (std::is_same_v<V, int32_t>) return EStringID::IntProperty;
			else if constexpr (std::is_same_v<V, float>) return EStringID::FloatProperty;
			else if constexpr (std::is_same_v<V, bool>) return EStringID::BoolProperty;
			else if constexpr (std::is_same_v<V, String>) return EStringID::StringProperty;
			else if constexpr (std::is_same_v<V, ObjectPtr<Object>>) return EStringID::ObjectProperty;
			else if constexpr (std::is_same_v<V, SoftObjectPtr<Object>>) return EStringID::SoftObjectProperty;
			else return EStringID::None;
		}
	};

#define DECLARE_CLASS(Class, Parent) \
public: \
	using Super = Parent; \
	static VeiM::ClassDescriptor* StaticClass(); \
	virtual VeiM::ClassDescriptor* GetClass() const override { return Class::StaticClass();} \
private: \
	static VeiM::ClassDescriptor* s_ClassDescriptor; \
public: \
	static void RegisterProperties(VeiM::ClassDescriptor* classDesc);

#ifdef VM_WITH_EDITOR
	#define REFLECTION_DECLARE_DEBUG_NAME(DNAME) s_ClassDescriptor->DebugName = String(#DNAME);
#else
#define REFLECTION_DECLARE_DEBUG_NAME(DNAME)
#endif

#define IMPLEMENT_CLASS(Class) \
	VeiM::ClassDescriptor* Class::s_ClassDescriptor = nullptr; \
	namespace \
	{ \
	bool _autoReg_##Class = []() \
		{ \
        VeiM::ClassRegistry::AddRegistrationCallback<Class>(); \
        return true; \
		}(); \
	} \
	VeiM::ClassDescriptor* Class::StaticClass() \
	{ \
		if (!s_ClassDescriptor) \
		{ \
			s_ClassDescriptor = new VeiM::ClassDescriptor(); \
			s_ClassDescriptor->Name = VeiM::StringID(#Class); \
			REFLECTION_DECLARE_DEBUG_NAME(Class) \
			s_ClassDescriptor->ParentClass = Super::StaticClass(); \
			s_ClassDescriptor->ConstructorFunc = []() -> VeiM::Object* {return new Class(); }; \
			VeiM::ClassRegistry::RegisterClass(s_ClassDescriptor); \
			Class::RegisterProperties(s_ClassDescriptor); \
		VM_CORE_TRACE("[Reflection] Registered '{0}'", VeiM::String(#Class)); \
		} \
		return s_ClassDescriptor; \
	}

	// TODO: Consider using GetClass instead of passing it
#define REGISTER_PROPERTY(Class, PropType, PropName) \
	{ \
		constexpr bool bIsBasicProperty = VeiM::EStringID::PropType != VeiM::EStringID::VectorProperty && VeiM::EStringID::PropType != VeiM::EStringID::MapProperty; \
		static_assert(bIsBasicProperty, \
		"Invalid property registration: 'REGISTER_PROPERTY' cannot be used for container types (VectorProperty/MapProperty). " \
		"Instead, use 'REGISTER_VECTOR_PROPERTY' for vectors or 'REGISTER_MAP_PROPERTY' for maps."	); \
		VeiM::PropertyDescriptor prop; \
		prop.Name = VeiM::StringID(#PropName); \
		prop.Type = VeiM::EStringID::PropType; \
		prop.Offset = offsetof(Class, PropName); \
		prop.Size = sizeof(decltype(Class::PropName)); \
		prop.bIsContainer = false; \
		prop.TypeIndex = std::type_index(typeid(decltype(Class::PropName))); \
		if constexpr (VeiM::EStringID::PropType == VeiM::EStringID::ObjectProperty || \
					  VeiM::EStringID::PropType == VeiM::EStringID::SoftObjectProperty) \
		{\
			prop.MarkReferencedObjects = [](void* instance, VeiM::GarbageCollector& gc) \
			{\
				auto& value = *reinterpret_cast<decltype(Class::PropName)*>( \
					reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
				if constexpr (HasGetMethod<decltype(Class::PropName)>) \
				{ \
					if (value.Get()) gc.MarkReachable(value.Get()); \
				}\
			};\
		} \
		s_ClassDescriptor->Properties.insert({VeiM::StringID(prop.Name), prop});\
	}

	// Vector property registration
#define REGISTER_VECTOR_PROPERTY(Class, InElementType, PropName) \
	{ \
		using VectorType = decltype(Class::PropName); \
		using ElementT = typename VectorType::value_type; \
		VeiM::PropertyDescriptor prop; \
		prop.Name = VeiM::StringID(#PropName); \
		prop.Type = VeiM::EStringID::VectorProperty; \
		prop.ElementType = VeiM::EStringID::InElementType; \
		prop.Offset = offsetof(Class, PropName); \
		prop.Size = sizeof(Class::PropName); \
		prop.bIsContainer = true; \
		prop.TypeIndex = std::type_index(typeid(VectorType)); \
		prop.ElementTypeIndex = std::type_index(typeid(ElementT)); \
		\
		prop.GetContainerSize = [](void* instance) -> size_t { \
			auto& container = *reinterpret_cast<VectorType*>( \
				reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
			return VeiM::ContainerHandler<VectorType>::GetSize(container); \
		}; \
		\
		prop.GetElementPtr = [](void* instance, size_t index) -> void* { \
			auto& container = *reinterpret_cast<VectorType*>( \
				reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
			return VeiM::ContainerHandler<VectorType>::GetElementPtr(container, index); \
		}; \
		\
		if constexpr (VeiM::EStringID::InElementType == VeiM::EStringID::ObjectProperty || \
			VeiM::EStringID::InElementType == VeiM::EStringID::SoftObjectProperty) { \
			prop.MarkReferencedObjects = [](void* instance, VeiM::GarbageCollector& gc) { \
				auto& container = *reinterpret_cast<VectorType*>( \
					reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
				VeiM::ContainerHandler<VectorType>::MarkReferencedObjects(container, gc); \
			}; \
		} \
		\
		s_ClassDescriptor->Properties.insert({VeiM::StringID(prop.Name), prop}); \
	}

// Map property registration
#define REGISTER_MAP_PROPERTY(Class, KeyType, ValueType, PropName) \
	{ \
		using MapType = decltype(Class::PropName); \
		using KeyT = typename MapType::key_type; \
		using ValueT = typename MapType::mapped_type; \
		VeiM::PropertyDescriptor prop; \
		prop.Name = VeiM::StringID(#PropName); \
		prop.Type = VeiM::EStringID::MapProperty; \
		prop.KeyType = VeiM::EStringID::KeyType; \
		prop.ElementType = VeiM::EStringID::ValueType; \
		prop.Offset = offsetof(Class, PropName); \
		prop.Size = sizeof(Class::PropName); \
		prop.bIsContainer = true; \
		prop.TypeIndex = std::type_index(typeid(MapType)); \
		prop.KeyTypeIndex = std::type_index(typeid(KeyT)); \
		prop.ElementTypeIndex = std::type_index(typeid(ValueT)); \
		\
		prop.GetContainerSize = [](void* instance) -> size_t { \
			auto& container = *reinterpret_cast<MapType*>( \
				reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
			return VeiM::ContainerHandler<MapType>::GetSize(container); \
		}; \
		\
		prop.MapIterator = [](void* instance, void* keyCallback, void* valueCallback) { \
			auto& container = *reinterpret_cast<MapType*>( \
				reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
			VeiM::ContainerHandler<MapType>::IterateMap(container, keyCallback, valueCallback); \
		}; \
		\
		if constexpr (VeiM::EStringID::ValueType == VeiM::EStringID::ObjectProperty || \
			VeiM::EStringID::ValueType == VeiM::EStringID::SoftObjectProperty) { \
			prop.MarkReferencedObjects = [](void* instance, VeiM::GarbageCollector& gc) { \
				auto& container = *reinterpret_cast<MapType*>( \
					reinterpret_cast<char*>(instance) + offsetof(Class, PropName)); \
				VeiM::ContainerHandler<MapType>::MarkReferencedObjects(container, gc); \
			}; \
		} \
		\
		s_ClassDescriptor->Properties.insert({VeiM::StringID(prop.Name), prop}); \
	}


/*
*	// Basic Properties
	REGISTER_PROPERTY(ExampleObject, StringProperty, StringProperty);
	REGISTER_PROPERTY(ExampleObject, FloatProperty, FloatProperty);
	// Object pointer Properties
	REGISTER_PROPERTY(ExampleObject, ObjectProperty, ObjectReference);
	REGISTER_PROPERTY(ExampleObject, SoftObjectProperty, SoftObjectReference);

	// Vector Properties
	REGISTER_VECTOR_PROPERTY(ExampleObject, IntProperty, IntArray);
	// Vector Object Properties
	REGISTER_VECTOR_PROPERTY(ExampleObject, ObjectProperty, ObjectPtrArray);

	// Map Properties
	REGISTER_MAP_PROPERTY(ExampleObject, IntProperty, IntProperty, IntToIntMap);
	// Map Object Properties
	REGISTER_MAP_PROPERTY(ExampleObject, StringIDProperty, ObjectProperty, StringIDToObjectMap);

*/
}