#include "AssetManager.h"

namespace VeiM
{

	void AssetManager::RegisterAsset(Object* asset)
	{
		if (asset && asset->IsAsset())
		{
			ObjectPath path = asset->GetObjectPath();
			if (path.IsValid())
			{
				m_LoadedAssets[path] = asset;
			}
		}
	}

	void AssetManager::UnregisterAsset(Object* asset)
	{
		if (asset && asset->IsAsset())
		{
			ObjectPath path = asset->GetObjectPath();
			auto it = m_LoadedAssets.find(path);
			if (it != m_LoadedAssets.end() && it->second == asset)
			{
				m_LoadedAssets.erase(it);
			}
		}
	}

	Object* AssetManager::FindAsset(const ObjectPath& path)
	{
		auto it = m_LoadedAssets.find(path);
		if (it != m_LoadedAssets.end())
		{
			return it->second;
		}
		return nullptr;
	}

}

