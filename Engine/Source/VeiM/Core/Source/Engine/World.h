#pragma once
#include "CoreDefines.h"

#include "Engine/CoreObject.h"
#include "Engine/Reflection.h"
#include "Engine/WeakObjectPtr.h"
#include "Engine/WorldSettings.h"
#include "Engine/EngineTypes.h"
#include "Utils/Math.h"
#include <vector>

// World.h
namespace VeiM
{
	using namespace Math;

	extern CORE_API class World* g_World;

	enum class EWorldType : uint8
	{
		Game = 0,
		Editor,
		EditorPlay,
		EditorTool
	};

	class Level;
	class Entity;
	class ServiceEntity;
	class GlobalGameState;
	class RenderScene;
	class PhysicsScene;
	class Controller;
	class TimerManager;
	class IScene;

	class CORE_API World final : public Object
	{
		DECLARE_CLASS(World, Object)
	public:
		World();
	public:
		ServiceEntity* GetServiceByClass(ClassDescriptor* serviceClass) const; // Not type safe
		template<typename T>
		T* GetServiceByClass() const { return CastObject<T>(GetServiceByClass(T::StaticClass())); }
		Level* GetCurrentLevel() const;
		WorldSettings* GetSettings();
		std::vector<WeakObjectPtr<Controller>>& GetControllers();
		Controller* GetController() const;
		virtual World* GetWorld() const override;
		void SetGlobalGameState(GlobalGameState* newGGS);
		GlobalGameState* GetGlobalGameState() const;
		template<typename T>
		T* GetGlobalGameState() const { return CastObject<T>(GetGlobalGameState()); }
		Entity* GetEntityByName(StringID name);

		void Tick(float deltaTime);
		void RunTickGroup(ETickGroup group);

		void InitializeNewWorld();
		void Initialize();
		void Destroy();
		void MarkAllPendingKill();
		void ClearAllComponents();
		void UpdateAllComponents();
		void Cleanup();
		void CleanupEntities();
		void StartDying();

		bool DestroyEntity(Entity* entity);
		bool RemoveEntity(Entity* entity);
		Entity* SpawnEntity(ClassDescriptor* spawnClass, const Transform& transform = Transform::Identity, bool bOverrideRootScale = false, StringID entityName = StringID(EStringID::None));
		Entity* SpawnEntity(ClassDescriptor* spawnClass, const glm::vec3& location = Math::Utils::ZeroVector, const glm::vec3& rotation = Math::Utils::OnesVector, bool bOverrideRootScale = false, StringID entityName = StringID(EStringID::None));
		void InitializeEntities();

		void BeginPlay();
		bool EndPlay();

		void SetBegunPlay(bool bBegun);
		bool HasBegunPlay() const;
		bool HasMarkedAllPendingKill() const;
		bool AreEntitiesInitialized() const;
		bool IsPaused();
		float GetDeltaTime() const;
		bool IsGameWorld() const;
		bool IsEditorWorld() const;
		bool IsToolWorld() const;

		void CreatePhysicsScene() {}
		PhysicsScene* GetPhysicsScene() const { return m_PScene; }
		void SetPhysicsScene(PhysicsScene* newPScene) {m_PScene = newPScene; }

		// float GetGravity() const;
		void AddController(Controller* controller);
		void RemoveController(Controller* controller);

		// OnBeginPlay and OnEndPlay delegates
		// AudioDevice Handle

					/* Object */
		virtual void StartDestroy() override;
		virtual void FinishDestroy() override;
#if 0
		void Serialize(serializer);

#endif
		virtual void Duplicate(Object* sourceObj, Object* destintationObj) override;
		static World* CreateWorld(const EWorldType worldType, StringID name, bool bAddToRoot);
		static World* GetDuplicateForEditorPlay(World* editorWorld);
	public:
		ObjectPtr<Level> CurrentLevel; 
		std::unordered_set<ObjectPtr<ServiceEntity>> LevelServices;
		IScene* Scene;

		// double LastRenderTime ?
		RenderScene* RScene;
		EWorldType WorldType;
		ETickGroup TickGroup;
		uint8 bInTick : 1;
		uint8 bInitialized : 1;
		uint8 bWasInitialized : 1;
		uint8 bEntitiesInitialized : 1;
		uint8 bIsDying : 1;
		uint8 bTickable : 1;
		
	private:
		uint8 m_bHasBegunPlay : 1;
		uint8 m_bMarkedAllPendingKill : 1;

		ObjectPtr<GlobalGameState> m_GlobalGameState;
		std::vector<WeakObjectPtr<Controller>> m_Controllers;
		PhysicsScene* m_PScene;
		TimerManager* m_TimerManager;
		float m_DeltaTime;
		float m_DeltaTimeUnscaled;

	};
}