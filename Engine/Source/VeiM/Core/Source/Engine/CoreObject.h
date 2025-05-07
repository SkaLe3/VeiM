#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"
#include "Engine/ObjectPath.h"
#include "Engine/ObjectPtr.h"
#include "Engine/GarbageCollector.h"
#include "Engine/ObjectCreatorSystem.h"

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <atomic>
#include <chrono>

// CoreObject.h
namespace VeiM
{
	class GarbageCollector;
	class AssetManager;
	class World;

	template <typename T> class WeakObjectPtr;
	template <typename T> class SoftObjectPtr;

	class ClassDescriptor;

	// TODO: Remove CORE_API from class and add only to functions and properties
	class CORE_API Object
	{
	public:
		enum Flags {
			FLAG_None = 0,
			FLAG_PENDING_KILL = 1 << 0,
			FLAG_ROOT = 1 << 1,
			FLAG_INITIALIZING = 1 << 2,
			FLAG_GARBAGE = 1 << 3,
			FLAG_TRANSIENT = 1 << 4,
			FLAG_ASSET = 1 << 5,
			FLAG_PROTOTYPE = 1 << 6
		};

	public:
		
		Object();
		virtual ~Object();

		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;


		Object* CreateComponent(StringID name, ClassDescriptor* instanceType/*, bool bTransient = false */);

		template<class InstanceType>
		InstanceType* CreateComponent(StringID name/*, bool bTransient = false */)
		{
			ClassDescriptor* instanceType = InstanceType::StaticClass();
			return static_cast<InstanceType*>(CreateComponent(name, instanceType));
		}

		/* Base */
		static ObjectCreatorSystem& GetCreatorSystem()
		{
			static ObjectCreatorSystem system;
			return system;
		}
		Object* GetCreator() const { return m_Creator.Get(); }
		void SetCreator(Object* newCreator);
		String GetName() const { return m_Name.ToString(); }

		Object* GetCreatorAs(ClassDescriptor* targetClass) const;
		template <typename T>
		T* GetCreatorAs() const
		{
			return (T*)GetCreatorAs(T::StaticClass());
		}

		bool IsA(ClassDescriptor* baseClass) const;

		template <typename T>
		bool IsA() const
		{
			return IsA(T::StaticClass());
		}
		virtual World* GetWorld() const;


		/* Reflection Interface */
		static ClassDescriptor* StaticClass();
		virtual ClassDescriptor* GetClass() const { return StaticClass(); }
		static void RegisterProperties(ClassDescriptor* classDesc) {  /* Base has no properties */ }

		/* Asset Path */
		virtual ObjectPath GetObjectPath() const { return ObjectPath(); }
		virtual void SetObjectPath(const ObjectPath& path) {}
		bool IsAsset() const { return HasFlag(FLAG_ASSET); }
		void SetAsAsset(bool isAsset);

		/* GC Interface */
		virtual void MarkReferencedObjects(GarbageCollector& gc);

		/* Object Flag management */
		void SetFlags(uint32 flags) { m_Flags = flags; }
		void SetFlag(Flags flag) { m_Flags |= flag; }
		void ClearFlag(Flags flag) { m_Flags &= ~flag; }
		bool HasFlag(Flags flag) const { return (m_Flags & flag) != 0; }

		bool IsPrototype(Flags flag = FLAG_PROTOTYPE) const;
		void MarkAsRoot();
		void UnmarkAsRoot();
		bool IsRoot() const { return HasFlag(FLAG_ROOT); }

		bool IsPendingKill() const { return HasFlag(FLAG_PENDING_KILL); }
		void MarkPendingKill();

		virtual void StartDestroy() {}
		virtual void FinishDestroy(){}

#if 0
		virtual void Serialize(Serializer& serializer);

#endif

		// Add Writing to config
	protected:
		virtual void OnMarkPendingKill() { /* Override in derived */ }

	private:
		Object* CreateComponent_Internal(Object* owner, StringID name, ClassDescriptor* instanceType);

	private:
	public: // Public for debug
		StringID m_Name;
		ObjectPtr<Object> m_Creator;
		uint32 m_Flags = 0;

		bool m_bIsMarkedForGC = false;

		template <typename T> friend class ObjectPtr;
		template <typename T> friend class WeakObjectPtr;
		template <typename T> friend class SoftObjectPtr;
		friend class GarbageCollector;
		friend class AssetManager;
	};


	
}