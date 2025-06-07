#include "Level.h"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Engine/ServiceEntity.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/Controller.h"

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
					entity->PreInitializeComponents();
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


	void Level::AddEntityToInputList(Entity* entity, const int32 index)
	{
		m_EntityInputList.emplace_back(entity, index);
	}

	void Level::ProcessNewInputInitWithInputList(Controller* controller)
	{
		std::vector<WeakObjectPtr<Controller>>& controllers = controller->GetWorld()->GetControllers();
		int32 myControllerIndex = -1;
		for (int32 i = 0; i < controllers.size(); i++)
		{
			if (controller == controllers[i])
			{
				myControllerIndex = i;
				break;
			}
		}

		if (myControllerIndex < 0)
		{
			return;
		}

		std::vector<Entity*> foundEntities;
		std::vector<std::pair<WeakObjectPtr<Entity>, int32>>::iterator newEnd = m_EntityInputList.end();
		for (int32 i = m_EntityInputList.size() - 1; i >= 0; i--)
		{
			auto& [obj, index] = m_EntityInputList[i];
			if (index == myControllerIndex)
			{
				if (obj.IsValid())
				{
					foundEntities.push_back(obj.Get());
				}
				std::swap(m_EntityInputList[i], m_EntityInputList.back());
				m_EntityInputList.pop_back();
			}
		}
		for (int32 i = foundEntities.size() - 1; i >= 0; i--)
		{
			Agent* agent = CastObject<Agent>(foundEntities[i]);
			if (agent)
			{
				controller->Posses(agent);
			}
			else
			{
				foundEntities[i]->EnableInput(controller);
			}
		}
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

	void Level::Duplicate(Object* sourceObj, Object* destintationObj)
	{
		Super::Duplicate(sourceObj, destintationObj);

		Level* sourceLevel = static_cast<Level*>(sourceObj);
		Level* destLevel = static_cast<Level*>(destintationObj);
		if (!sourceLevel || !destLevel)
			return;
		VM_CORE_TRACE("[Object] Duplicating Level: '{0}'", sourceLevel->GetName());

		destLevel->OwningWorld = destLevel->GetCreatorAs<World>();

		for (auto& entityPtr : sourceLevel->Entities)
		{
			if (entityPtr)
			{
				Entity* entity = entityPtr.Get();
				Entity* duplicatedEntity = static_cast<Entity*>(entity->DuplicateObject(
					entity,
					destLevel,
					entity->m_Name,
					entity->GetClass()
				));

				destLevel->AddEntity(duplicatedEntity);

				ServiceEntity* serviceEntity = CastObject<ServiceEntity>(duplicatedEntity);

				if (serviceEntity)
				{
					destLevel->OwningWorld->LevelServices.insert(ObjectPtr(serviceEntity));
				}
			}
		}
	}

	void Level::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);


		// TODO: Reigster
	}
}

