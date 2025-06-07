#pragma once
#include "CoreDefines.h"
#include "Engine/ObjectPtr.h"

namespace VeiM
{


	template <typename T>
	class WeakObjectPtr : public ObjectPtrBase
	{
	public:
		WeakObjectPtr() : m_Ptr(nullptr) {}

		// Raw pointer c-tor
		explicit WeakObjectPtr(T* rawPtr) : m_Ptr(rawPtr)
		{
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}

		// Strong Object pointer c-tor
		explicit WeakObjectPtr(const ObjectPtr<T>& strongPtr) : m_Ptr(strongPtr.Get())
		{
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}

		// Copy c-tor
		WeakObjectPtr(const WeakObjectPtr& other) : m_Ptr(other.m_Ptr)
		{
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}
		// Move Copy c-tor
		WeakObjectPtr(WeakObjectPtr&& other) noexcept : m_Ptr(other.m_Ptr)
		{
			//other.m_Ptr = nullptr;
			if (m_Ptr)
			{
				RegisterWithGC();
			}
		}
		// d-tor
		~WeakObjectPtr()
		{
			if (m_Ptr)
			{
				UnregisterWithGC();
			}
		}


		// Copy operator
		WeakObjectPtr& operator=(const WeakObjectPtr& other)
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
		// Move Copy operator
		WeakObjectPtr& operator=(WeakObjectPtr&& other) noexcept
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

		// Copy operator from Strong Object pointer
		WeakObjectPtr& operator=(const ObjectPtr<T>& objPtr)
		{
			if (m_Ptr) 
			{
				UnregisterWithGC();
			}

			m_Ptr = objPtr.Get();

			if (m_Ptr) 
			{
				RegisterWithGC();
			}

			return *this;
		}

		// Copy operator from raw
		WeakObjectPtr& operator=(T* ptr)
		{
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


		ObjectPtr<T> Lock() const
		{
			return IsValid() ? ObjectPtr<T>(m_Ptr) : ObjectPtr<T>();
		}

		bool IsValid() const { return m_Ptr != nullptr && !m_Ptr->IsPendingKill(); }

		T* operator->() const { return m_Ptr; }
		T& operator*() const { return *m_Ptr; }
		explicit operator bool() const { return IsValid(); }

		T* Get() const { return m_Ptr; }

		void Reset()
		{
			if (m_Ptr) {
				UnregisterWithGC();
				m_Ptr = nullptr;
			}
		}

		void Invalidate() {
			m_Ptr = nullptr;
		}
		operator T* () const { return Get(); }

		// Comparison operators
		bool operator==(const WeakObjectPtr& other) const { return m_Ptr == other.m_Ptr; }
		bool operator!=(const WeakObjectPtr& other) const { return m_Ptr != other.m_Ptr; }
		bool operator==(const ObjectPtr<T>& other) const { return m_Ptr == other.Get(); }
		bool operator!=(const ObjectPtr<T>& other) const { return m_Ptr != other.Get(); }
		bool operator==(std::nullptr_t) const { return m_Ptr == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_Ptr != nullptr; }

	protected:
		void RegisterWithGC() override
		{
			PointerRegistry::RegisterWeakPtr(static_cast<Object*>(GetRawObjectPtr()), this);
		}

		void UnregisterWithGC() override
		{
			PointerRegistry::UnregisterWeakPtr(static_cast<Object*>(GetRawObjectPtr()), this);
		}
		virtual void* GetRawObjectPtr() const override { return m_Ptr; }
	private:
		void CheckDerived() const {
			static_assert(DerivedFromObject<T>, "T must derive from Object");
		}
	private:

		mutable T* m_Ptr = nullptr;
	};

	// Implementation of AsWeak() method for ObjectPtr
	template <typename T>
	WeakObjectPtr<T> ObjectPtr<T>::AsWeak() const
	{
		return WeakObjectPtr<T>(m_Ptr);
	}
}