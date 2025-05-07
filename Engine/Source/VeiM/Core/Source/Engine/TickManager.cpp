#include "TickManager.h"
#include "Engine/World.h"
#include "Engine/Level.h"

namespace VeiM
{

	TickFunction::TickFunction()
		: m_TickState(EState::Enabled),
		TickGroup(ETickGroup::Default),
		TickPeriod(0.f),
		bTickable(false),
		bTickOnStart(false),
		bTickInPause(false),
		m_bRegistered(false),
		m_LastTickTimeSecs(-1.f)
	{
	}

	TickFunction::~TickFunction()
	{
		Unregister();
	}


	void TickFunction::Register(Level* level)
	{
		if (!IsRegistered())
		{
			TickManager::Get().AddTickFunction(level, this);
			m_bRegistered = true;
		}
	}

	void TickFunction::Unregister()
	{
		if (IsRegistered())
		{
			TickManager::Get().RemoveTickFunction(this);
			m_bRegistered = false;
		}
	}

	void TickFunction::SetEnable(bool bEnabled)
	{
		if (IsRegistered())
		{
			if (bEnabled && m_TickState == EState::Disabled)
			{
				m_LevelTick->RemoveTickFunction(this);
				m_TickState = (bEnabled ? EState::Enabled : EState::Disabled);
				m_LevelTick->AddTickFunction(this);
			}
		}
	}

	void TickFunction::SetPeriod(float newPeriod)
	{
		// Not supported yet
	}


	LevelTick* TickManager::GetLevelTick(Level* level)
	{
		if (level->TickLevel == nullptr)
		{
			level->TickLevel = CreateLevelTick();
		}
		return level->TickLevel;
	}

	void TickManager::AddTickFunction(Level* level, TickFunction* tickFunction)
	{
		LevelTick* levelTick = GetLevelTick(level);
		levelTick->AddTickFunction(tickFunction);
		tickFunction->m_LevelTick = levelTick;
	}

	void TickManager::RemoveTickFunction(TickFunction* tickFunction)
	{
		LevelTick* levelTick = tickFunction->m_LevelTick;
		levelTick->RemoveTickFunction(tickFunction);
	}

	bool TickManager::HasTickFunction(Level* level, TickFunction* tickFunction)
	{
		LevelTick* levelTick = GetLevelTick(level);
		return levelTick && levelTick->HasTickFunction(tickFunction);
	}

	void TickManager::StartFrame(World* world, float deltaTime, Level* level)
	{
		m_TickGroup = ETickGroup(0);
		m_DeltaTime = deltaTime;
		m_World = world;
		m_LevelTick = level->TickLevel;
		//TODO: level->StartFrame() - process cooldown for fixed ticks
	}

	void TickManager::EndFrame()
	{
		//TODO: level->EndFrame() - also process colldown
		m_World = nullptr;
		m_LevelTick = nullptr;
	}

	void TickManager::RunTickGroup(ETickGroup group)
	{
		if (!m_LevelTick) return;
	
		m_LevelTick->RunEnabledTickFunctions(m_DeltaTime);
		m_TickGroup = ETickGroup((int32)m_TickGroup + 1);

	}

	void LevelTick::AddTickFunction(TickFunction* tickFunction)
	{
		if (tickFunction->m_TickState == TickFunction::EState::Enabled)
		{
			m_EnabledTickFunctions.insert(tickFunction);
		}
		else
		{
			m_DisabledTickFunctions.insert(tickFunction);
		}
	}

	void LevelTick::RemoveTickFunction(TickFunction* tickFunction)
	{
		switch (tickFunction->m_TickState)
		{
		case TickFunction::EState::Enabled:
			m_EnabledTickFunctions.erase(tickFunction);
			break;
		case TickFunction::EState::Disabled:
			m_DisabledTickFunctions.erase(tickFunction);
			break;
		default:
			break;
		}
	}

	bool LevelTick::HasTickFunction(TickFunction* tickFunction)
	{
		return m_EnabledTickFunctions.count(tickFunction) || m_DisabledTickFunctions.count(tickFunction);
	}

	void LevelTick::RunEnabledTickFunctions(float deltaTime)
	{
		for (TickFunction* tick : m_EnabledTickFunctions)
		{
			tick->RunTick(deltaTime);
		}
	}

}

