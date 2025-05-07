#pragma once
#include "CoreDefines.h"
#include "Engine/ObjectPath.h"
#include "Engine/AssetManager.h"

namespace VeiM
{

	template <typename T>
	class SoftObjectPtr
	{
	public:
		SoftObjectPtr() = default;

		// Construct from asset path
		explicit SoftObjectPtr(const ObjectPath& path) : m_Path(path) {}

		explicit SoftObjectPtr(T* ptr) : m_StrongPtr(ptr)
		{
			if (ptr)
			{
				m_Path = ptr->GetObjectPath();
			}
		}
		
		bool IsValid() const { return m_Path.IsValid(); }// Check if the path is valid	
		bool IsLoaded() const { return m_StrongPtr.IsValid(); }// Check if the asset is loaded
		bool IsPending() const { return IsValid() && !IsLoaded(); }

		T* Get()
		{
			if (IsLoaded())
				return m_StrongPtr.Get();
			else
				nullptr;
		}
		const ObjectPath& GetPath() const { return m_Path; }
		void SetPath(const ObjectPath& path)
		{
			m_Path = path;
			m_StrongPtr->Reset();
		}

		// Copy and move constructors
		SoftObjectPtr(const SoftObjectPtr& other) = default;
		SoftObjectPtr(SoftObjectPtr&& other) noexcept = default;

		// Copy and move assignment
		SoftObjectPtr& operator=(const SoftObjectPtr& other) = default;
		SoftObjectPtr& operator=(SoftObjectPtr&& other) noexcept = default;

		// Try to load synchronously
		T* LoadSynchronous() const
		{
			if (!m_Path.IsValid())
			{
				return nullptr;
			}

			if (m_StrongPtr.IsValid())
			{
				return m_StrongPtr;
			}

			// Load from asset manager
			T* loadedAsset = AssetManager::Get().LoadAsset<T>(m_Path);
			if (loadedAsset)
			{
				m_StrongPtr = ObjectPtr<T>(loadedAsset);
				return loadedAsset;
			}

			return nullptr;
		}

		// Load asynchronously with callback
		void LoadAsync(std::function<void(T*)> callback) const
		{
			if (!m_Path.IsValid())
			{
				if (callback)
				{
					callback(nullptr);
				}
				return;
			}

			if (m_StrongPtr.IsValid())
			{
				if (callback)
				{
					callback(m_StrongPtr.Get());
				}
				return;
			}

			AssetManager::Get().LoadAssetAsync<T>(m_Path, [this, callback](T* loadedAsset)
				{
					if (loadedAsset)
					{
						m_StrongPtr = loadedAsset;
						if (callback)
						{
							callback(loadedAsset);
						}
					}
					else if (callback)
					{
						callback(nullptr);
					}
				});
		}

		// Comparison operators
		bool operator==(const SoftObjectPtr& other) const { return m_Path == other.m_Path; }
		bool operator!=(const SoftObjectPtr& other) const { return m_Path != other.m_Path; }
		bool operator==(std::nullptr_t) const { return !m_Path.IsValid(); }
		bool operator!=(std::nullptr_t) const { return m_Path.IsValid(); }

	private:
		ObjectPath m_Path;
		ObjectPtr<T> m_StrongPtr;

	};
}