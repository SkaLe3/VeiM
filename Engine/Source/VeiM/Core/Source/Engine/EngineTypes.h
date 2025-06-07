#pragma once
#include "CoreDefines.h"

#include <glm/glm.hpp>

// EngineTypes.h
namespace VeiM
{
	// TODO: Add macros for reflection in editor
	// Make editable in defaults only


	enum class ETickGroup : int32
	{
		Default = 0,
		MaxGroup
	};

	class Level;
	class Entity;
	class Component;
	class LevelTick;
	class TickManager;

	struct CORE_API TickFunction
	{
	private:
		enum class EState : uint8
		{
			Enabled,
			Disabled,
			Scheduled // like CoolingDown
		};
		EState m_TickState;

	public:
		TickFunction();
		virtual ~TickFunction();

		void Register(Level* level);
		void Unregister();
		bool IsRegistered() const { return m_bRegistered; }

		void SetEnable(bool bEnabled);
		bool IsEnabled() const { return m_TickState != EState::Disabled; }

		void SetPeriod(float newPeriod);
		float GetLastTickTime() const { return m_bRegistered ? m_LastTickTimeSecs : -1.f; }

		virtual void RunTick(float deltaTime) = 0;

	public:
		ETickGroup TickGroup;
		float TickPeriod;

		uint8 bTickable : 1;
		uint8 bTickOnStart : 1;
		uint8 bTickInPause : 1;

	private:
		uint8 m_bRegistered : 1;

		float m_LastTickTimeSecs;

		LevelTick* m_LevelTick;

		friend LevelTick;
		friend TickManager;
	};



	struct EntityTickFunction : public TickFunction
	{
	public:
		Entity* TargetObject;

		virtual void RunTick(float deltaTime) override;
	};


	struct ComponentTickFunction : public TickFunction
	{
	public:
		Component* TargetObject;

		virtual void RunTick(float deltaTime) override;
	};

	enum class EAttachmentRule : uint8
	{
		/* Current relative transform becomes relative to parent */
		Relative,
		/* Calculate relative transform to maintain same world position */
		World
	};


	struct CORE_API AttachmentTransformRules
	{
		EAttachmentRule LocationRule;
		EAttachmentRule RotationRule;
		EAttachmentRule ScaleRule;

		AttachmentTransformRules(EAttachmentRule inRule)
			: LocationRule(inRule),
			RotationRule(inRule),
			ScaleRule(inRule)
		{
		}

		AttachmentTransformRules(EAttachmentRule locRule, EAttachmentRule rotRule, EAttachmentRule scaleRule)
			: LocationRule(locRule),
			RotationRule(rotRule),
			ScaleRule(scaleRule)
		{
		}

		static AttachmentTransformRules TransformRelative;
		static AttachmentTransformRules TransformWorld;

	};

	namespace EInputEvent
	{

		enum Type : uint32
		{
			Pressed = 0,
			Released = 1,
			Repeat = 2,
			DoubleClick = 3,
			Axis = 4,
			Max
		};
	}
	enum class EMouseCapture : uint8
	{
		Ignore,
		Always,
		RightMouseButton
	};
	struct KeyState
	{
		glm::vec2 Value;
		glm::vec2 RawValue;
		bool bDown;
		bool bDownPrev;
		bool bConsumed;
		bool bFresh;
		// There might be multiple events for one key per frame, for example with long frame time, or when Doubleclick adds Pressed event
		std::vector<EInputEvent::Type> EventAccum; 
		std::vector<EInputEvent::Type> EventAccumSave;
	};
}