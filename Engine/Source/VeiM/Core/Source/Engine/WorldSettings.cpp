#include "WorldSettings.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/Entity.h"

namespace VeiM
{

	void WorldSettings::NotifyBeginPlay()
	{
		World* world = g_World;
		if (!world->HasBegunPlay())
		{
			for (ObjectPtr<Entity> entity : world->CurrentLevel->Entities)
			{
				if (entity.IsValid())
				{
					entity->ProcessBeginPlay();
				}
			}

			world->SetBegunPlay(true);
		}
	}

}

