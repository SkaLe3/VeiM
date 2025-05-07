#pragma once
#include "CoreDefines.h"

#include "Engine/ObjectPath.h"
#include "Engine/CoreObject.h"

namespace VeiM
{

	class CORE_API AssetManager
	{
	public:
		static AssetManager& Get()
		{
			static AssetManager instance;
			return instance;
		}

		void RegisterAsset(Object* asset);
		void UnregisterAsset(Object* asset);

		Object* FindAsset(const ObjectPath& path);

		// Load an asset by path
		template<typename T>
		T* LoadAsset(const ObjectPath& path)
		{
			// First check if already loaded
			Object* existing = FindAsset(path);
			if (existing)
			{
				// Check if it's the right type
				if (existing->GetClass()->IsChildOf(T::StaticClass()))
				{
					return static_cast<T*>(existing);
				}
				return nullptr; // Wrong type
			}

			// Load the asset (placeholder implementation)
			T* loadedAsset = LoadAssetFromStorage<T>(path);
			if (loadedAsset)
			{
				loadedAsset->SetAsAsset(true);
				loadedAsset->SetObjectPath(path);
				RegisterAsset(loadedAsset);
			}
			return loadedAsset;
		}

		// Asynchronously load an asset by path
		template<typename T>
		void LoadAssetAsync(const ObjectPath& path, std::function<void(T*)> callback)
		{
			// Check if already loaded
			Object* existing = FindAsset(path);
			if (existing)
			{
				// Check if it's the right type
				if (existing->GetClass()->IsChildOf(T::StaticClass()))
				{
					if (callback)
					{
						callback(static_cast<T*>(existing));
					}
					return;
				}

				if (callback)
				{
					callback(nullptr); // Wrong type
				}
				return;
			}

			// Queue async load (placeholder - implement based on your threading model)
			// In a real implementation, this would dispatch to a worker thread
			// For now, just simulate async with immediate loading
			T* loadedAsset = LoadAssetFromStorage<T>(path);
			if (loadedAsset)
			{
				loadedAsset->SetAsAsset(true);
				loadedAsset->SetObjectPath(path);
				RegisterAsset(loadedAsset);
			}

			if (callback)
			{
				callback(loadedAsset);
			}
		}

	private:
		AssetManager() = default;

		// Placeholder for actual asset loading
		template<typename T>
		T* LoadAssetFromStorage(const ObjectPath& path)
		{
			// This would be implemented with your actual asset loading logic
			// For now, return nullptr as a placeholder
			return nullptr;
		}

	private:
		std::unordered_map<ObjectPath, Object*, ObjectPath::Hash> m_LoadedAssets;
	};
}