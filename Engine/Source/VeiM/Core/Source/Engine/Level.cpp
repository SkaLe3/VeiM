#include "Level.h"
#include "Engine/Entity.h"
#include "Engine/World.h"

namespace VeiM
{
	IMPLEMENT_CLASS(Level);

	bool Level::AddEntity(Entity* entity)
	{
		Entities.emplace_back(entity);
		return true;
	}

	void Level::InitializeEntities()
	{
		for (ObjectPtr<Entity> entity : Entities)
		{
			if (entity.IsValid())
			{
				if (!entity->IsInitialized())
				{
					entity->InitializeComponents();
				}
			}
		}

		if (OwningWorld->HasBegunPlay())
		{
			for (ObjectPtr<Entity> entity : Entities)
			{
				if (entity.IsValid())
				{
					entity->ProcessBeginPlay();
				}
			}
		}
	}

	void Level::UpdateAllComponents()
	{
		for (ObjectPtr<Entity> entity : Entities)
		{
			if (entity.IsValid())
			{
				if (!entity->AllComponentsRegistered())
				{
					entity->RegisterAllComponents();
				}
			}
		}
	}

	void Level::ClearAllComponents()
	{
		for (ObjectPtr<Entity> entity : Entities)
		{
			if (entity)
			{
				entity->UnregisterAllComponents();
			}
		}
	}

	void Level::Cleanup()
	{

	}


	void Level::SetWorldSettings(SharedPtr<WorldSettings> settings)
	{
		m_Settings = settings;
	}


	void Level::MarkReferencedObjects(GarbageCollector& gc)
	{
		Super::MarkReferencedObjects(gc);
		gc.MarkReachableArray(Entities);
	}

	void Level::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);


		// TODO: Reigster
	}
}

