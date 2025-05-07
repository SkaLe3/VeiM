#pragma once
#include "CoreDefines.h"
#include "Engine/EngineTypes.h"

#include <unordered_set>

// TickManager.h
namespace VeiM
{

	class LevelTick
	{
	public:
		void AddTickFunction(TickFunction* tickFunction);
		void RemoveTickFunction(TickFunction* tickFunction);
		bool HasTickFunction(TickFunction* tickFunction);
		void RunEnabledTickFunctions(float deltaTime);

	private:
		std::unordered_set<TickFunction*> m_EnabledTickFunctions;
		std::unordered_set<TickFunction*> m_DisabledTickFunctions;

	};

	class World;

	class TickManager
	{
	public:
		static TickManager& Get()
		{
			static TickManager instance;
			return instance;
		}
		LevelTick* CreateLevelTick() { return new LevelTick; }
		LevelTick* GetLevelTick(Level* level);

		void AddTickFunction(Level* level, TickFunction* tickFunction);
		void RemoveTickFunction(TickFunction* tickFunction);
		bool HasTickFunction(Level* level, TickFunction* tickFunction);


		void StartFrame(World* world, float deltaTime, Level* level);
		void EndFrame();
		void RunTickGroup(ETickGroup group);

	private:

		World* m_World = nullptr;
		float m_DeltaTime = 0.0;
		ETickGroup m_TickGroup = ETickGroup::Default;
		LevelTick* m_LevelTick;
	};
}