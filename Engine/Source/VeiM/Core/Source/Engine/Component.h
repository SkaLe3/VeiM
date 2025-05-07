#pragma once
#include "CoreDefines.h"
#include "Engine/Reflection.h"
#include "Engine/CoreObject.h"
#include "Engine/EngineTypes.h"

#include <functional>

// Component.h
namespace VeiM
{
	class Entity;
	class World;


	class CORE_API Component : public Object
	{
		DECLARE_CLASS(Component, Object)
	public:
		Component();
		virtual ~Component();

		virtual void Initialize();
		virtual void Uninitialize();
		bool IsInitialized() const { return m_bInitialized; }
		bool IsCreated() const { return m_bCreated; }

		virtual void BeginPlay();
		virtual void EndPlay();

		virtual void TickComponent(float deltaTime) {}
		void RegisterTickFunctions(bool bRegister);
		virtual void SetTickEnable(bool bEnabled);
		virtual bool IsTickEnabled() const { return DefaultComponentTick.IsEnabled(); }
		void SetTickPeriod(float period) { DefaultComponentTick.TickPeriod = period; }
		float GetTickPeriod() const { return DefaultComponentTick.TickPeriod; }


		void RegisterInWorld(World* world);
		
		inline bool IsRenderStateDirty() const { return m_bRenderStateDirty; }
		inline bool IsRenderTransformDirty() const { return m_bRenderTransformDirty; }
		void MarkRenderStateDirty();
		void MarkRenderTransformDirty();
		inline bool IsRenderStateCreated() const { return m_bRenderStateCreated; }
		inline bool IsPhysicsStateCreated() const { return m_bPhysicsStateCreated; }


		void RegisterComponent();
		void UnregisterComponent();
		bool IsRegistered() const { return m_bRegistered; }
		virtual void Destroy();

		virtual void OnCreated();
		virtual void OnDetroyed(bool detroyInChain);

#if 0
		class RenderScene* GetScene() const;
#endif
		Level* GetLevel() const;

		virtual void OnEntityVisualsChanged() { MarkRenderStateDirty(); }

		// Owner management
		Entity* GetOwner() const;
		template<typename T> 
		T* GetOwner() const { return CastObject<T>(GetOwner()); }
		virtual World* GetWorld() const override final { return m_WorldCached ? m_WorldCached : GetWorldInternal(); }

		bool HasTag(StringID tag) const;
		bool HasBegunPlay() const { return m_bHasBegunPlay; }

		bool IsActive() const { return m_bIsActive; }
		void SetActive(bool active);
		void ToggleActive() { SetActive(!IsActive()); }

		/* Object */
		virtual void StartDestroy() override;
#if 0
		void Serialize(Serializer& serializer);

#endif
		/*        */

	protected:
		virtual void OnRegister();
		virtual void OnUnregister();
		virtual bool NeedsRenderState() const { return false; }
		virtual bool NeedsPhysicsState() const { return false; }
		virtual void CreateRenderState();
		virtual void DestroyRenderState();
		void CreatePhysicsState(); //May be make public
		void DestroyPhysicsState();
		virtual void RegisterTickFunction(bool bRegister);
	private:
		World* GetWorldInternal() const;

		void Register();
		void Unregister();

	public:
		ComponentTickFunction DefaultComponentTick;
		std::vector<StringID> Tags;

		bool bCreatedNatively;
		// Consider adding delegates for OnActivation and Deactivation

	protected:
		uint8 m_bRegistered : 1;
		uint8 m_bRenderStateCreated : 1;
		uint8 m_bPhysicsStateCreated : 1;

	private:
		uint8 m_bRenderStateDirty : 1;
		uint8 m_bRenderTransformDirty : 1;

		uint8 m_bIsActive : 1;                                     
		uint8 m_bCreated : 1;
		uint8 m_bInitialized : 1;
		uint8 m_bHasBegunPlay : 1;
		uint8 m_bDying : 1;
		uint8 m_bTicksRegistered : 1;
		// bOnlyVisual
	private:
		Entity* m_OwnerCached;
		World* m_WorldCached;

		friend class Entity;
	};

}
