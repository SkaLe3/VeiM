#pragma once
#include "CoreDefines.h"

#include "Engine/CoreObject.h"
#include "Engine/Reflection.h"
#include "Engine/Entity.h"
#include "Engine/ObjectPtr.h"

// Level.h
namespace VeiM
{
	class World;
	class LevelTick;
	class WorldSettings;
	class Controller;

	class CORE_API Level : public Object
	{
		DECLARE_CLASS(Level, Object)
	public:
		bool AddEntity(Entity* entity);
		void InitializeEntities();
		void UpdateAllComponents();
		void ClearAllComponents();

		void Cleanup();

		void AddEntityToInputList(Entity* entity, const int32 index);
		void ProcessNewInputInitWithInputList(Controller* controller);
		WorldSettings* GetWorldSettings() const { return m_Settings.get(); }
		void SetWorldSettings(SharedPtr<WorldSettings> settings);

		virtual void MarkReferencedObjects(GarbageCollector& gc) override;

		virtual void Duplicate(Object* sourceObj, Object* destintationObj) override;
	public:
		std::vector<ObjectPtr<Entity>> Entities;
		ObjectPtr<World> OwningWorld; //TODO: Register
		LevelTick* TickLevel;
	private:
		std::vector<std::pair<WeakObjectPtr<Entity>, int32>> m_EntityInputList;
		SharedPtr<WorldSettings> m_Settings;
	};
}