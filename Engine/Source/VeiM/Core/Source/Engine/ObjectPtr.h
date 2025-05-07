#pragma once
#include "CoreDefines.h"
#include "Engine/ObjectPtrBase.h"
#include "Engine/GarbageCollector.h"

#include <type_traits> 

namespace VeiM
{
	class Object; 
	class ClassDescriptor;



	template <typename T>
	concept DerivedFromObject = std::is_base_of_v<Object, T>;

	template <typename T> class WeakObjectPtr;

	template <typename T>
	class ObjectPtr : public ObjectPtrBase
	{
	public:
		ObjectPtr() : m_Ptr(nullptr) {}

		explicit ObjectPtr(T* rawPtr) : m_Ptr(rawPtr)
		{
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}

		ObjectPtr(const ObjectPtr& other) : m_Ptr(other.m_Ptr)
		{
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}

		ObjectPtr(ObjectPtr&& other) noexcept : m_Ptr(other.m_Ptr)
		{
			//other.m_Ptr = nullptr;
			if (m_Ptr) {
				RegisterWithGC();
			}
		}

		~ObjectPtr()
		{
			if (m_Ptr)
			{
				UnregisterWithGC();
			}
		}


		ObjectPtr& operator=(const ObjectPtr& other)
		{
			if (this == &other) return *this;

			if (m_Ptr)
			{
				UnregisterWithGC();
			}
			m_Ptr = other.m_Ptr;

			if (m_Ptr)
			{
				RegisterWithGC();
			}
			return *this;
		}

		ObjectPtr& operator=(ObjectPtr&& other) noexcept
		{
			if (this == &other) return *this;

			if (m_Ptr) {
				UnregisterWithGC();
			}

			m_Ptr = other.m_Ptr;


			if (m_Ptr) {
				RegisterWithGC();
			}

			return *this;
		}

		// Conversions
		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		ObjectPtr(const ObjectPtr<U>& other) : m_Ptr(other.Get())
		{
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		ObjectPtr& operator=(const ObjectPtr<U>& other)
		{
			if (m_Ptr != other.Get())
			{
				UnregisterWithGC();
				m_Ptr = other.Get();
				RegisterWithGC();
			}
			return *this;
		}

		ObjectPtr& operator=(T* ptr)
		{
			if (m_Ptr == ptr) return *this;


			if (m_Ptr)
			{
				UnregisterWithGC();
			}
			m_Ptr = ptr;
			if (m_Ptr)
			{
				RegisterWithGC();
			}

			return *this;
		}

		// Access operators
		T* operator->() const
		{
			VM_ASSERT(IsValid(), "Accessing invalid object pointer");
			return m_Ptr;
		}
		T& operator*() const
		{
			VM_ASSERT(IsValid(), "Accessing invalid object pointer");
			return *m_Ptr;
		}

		// Comparison operators
		bool operator==(const ObjectPtr& other) const { return m_Ptr == other.m_Ptr; }
		bool operator!=(const ObjectPtr& other) const { return m_Ptr != other.m_Ptr; }
		bool operator==(const T* other) const { return m_Ptr == other; }
		bool operator!=(const T* other) const { return m_Ptr != other; }
		bool operator==(std::nullptr_t) const { return m_Ptr == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_Ptr != nullptr; }

		// Check if valid
		explicit operator bool() const { return IsValid(); }
		bool IsValid() const { return m_Ptr != nullptr && !m_Ptr->IsPendingKill(); }

		T* Get() const { return m_Ptr; }
		void Reset()
		{
			if (m_Ptr)
			{
				UnregisterWithGC();
				m_Ptr = nullptr;
			}
		}
		void Invalidate() {
			m_Ptr = nullptr;
		}
		WeakObjectPtr<T> AsWeak() const;

		bool IsA(const ClassDescriptor* baseClass) const
		{
			if (m_Ptr->GetClass())
			{
				return m_Ptr->GetClass()->IsChildOf(baseClass);
			}
			return false;
		}
		template <typename T> bool IsA() const
		{
			return IsA(T::StaticClass());
		}

	protected:
		virtual void RegisterWithGC() override
		{
			PointerRegistry::RegisterStrongPtr(static_cast<Object*>(GetRawObjectPtr()), this);
		}

		virtual void UnregisterWithGC() override
		{
			PointerRegistry::UnregisterStrongPtr(static_cast<Object*>(GetRawObjectPtr()), this);
		}

		virtual void* GetRawObjectPtr() const override { return m_Ptr; }
	private:
		void CheckDerived() const {
			static_assert(DerivedFromObject<T>, "T must derive from Object");
		}
	private:

		T* m_Ptr = nullptr;
	};


}

namespace std {
	template<typename T>
	struct hash<VeiM::ObjectPtr<T>> {
		size_t operator()(const VeiM::ObjectPtr<T>& ptr) const noexcept {
			return std::hash<T*>{}(ptr.Get());
		}
	};
}