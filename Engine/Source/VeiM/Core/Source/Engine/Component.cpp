#include "Component.h"
#include "Engine/EngineTypes.h"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Engine/Level.h"

namespace VeiM
{
	IMPLEMENT_CLASS(Component)

	Component::Component()
		:m_bRegistered(false),
		m_bRenderStateCreated(false),
		m_bPhysicsStateCreated(false),
		m_bRenderStateDirty(false),
		m_bRenderTransformDirty(false),
		m_bIsActive(true),
		m_bCreated(false),
		m_bInitialized(false),
		m_bHasBegunPlay(false),
		m_bDying(false),
		m_bTicksRegistered(false),
		m_OwnerCached(nullptr),
		m_WorldCached(nullptr)
	{
		m_Creator = GetCreatorAs<Entity>();

		DefaultComponentTick.TickGroup = ETickGroup::Default;
		DefaultComponentTick.bTickOnStart = true;
		DefaultComponentTick.bTickable = false;
		DefaultComponentTick.SetEnable(false);
	}

	Component::~Component()
	{
	}


	void Component::SetTickEnable(bool bEnabled)
	{
		if (DefaultComponentTick.bTickable && !IsPrototype())
		{
			DefaultComponentTick.SetEnable(bEnabled);
		}
	}

	void Component::RegisterTickFunctions(bool bRegister)
	{
		if (bRegister)
		{
			if (!m_bTicksRegistered)
			{
				RegisterTickFunction(bRegister);
				m_bTicksRegistered = true;
			}
		}
	}

	void Component::Initialize() 
	{
		m_bInitialized = true;
	}


	void Component::Uninitialize()
	{
		m_bInitialized = false;
	}

	void Component::BeginPlay()
	{
		m_bHasBegunPlay = true;
	}


	void Component::EndPlay()
	{
		m_bHasBegunPlay = false;
	}

	void Component::RegisterInWorld(World* world)
	{
		if (m_bRegistered)
		{
			VM_CORE_WARN("Component already registered");
			return;
		}
		if (world == nullptr)
			return;

		Entity* owner = GetOwner();
		if (!m_bCreated)
		{
			OnCreated();
		}
		m_WorldCached = world;
		Register();
		if (!world->IsGameWorld()) // Maybe delete this
		{
			RegisterTickFunctions(true);
		}
		else if (owner == nullptr)
		{
			if (!m_bInitialized)
				Initialize();
			RegisterTickFunctions(true);
		}
		else
		{
			owner->ProcessComponentRegistration(this);
		}

#if 0
		// TODO: Implement input logic!
		if (owner && owner->InputComponent && SomeGlobalInputSettings->EnableComponentInputBindings)
		{
			// Bind input delegates to component
		}
#endif
	}

	void Component::MarkRenderStateDirty()
	{
		m_bRenderStateDirty = true;
		// Add Broadcast event
	}

	void Component::MarkRenderTransformDirty()
	{
		m_bRenderTransformDirty = true;
		// Add broadcast event;
	}

	void Component::RegisterComponent()
	{
		Entity* owner = GetOwner();
		World* ownerWorld = owner ? owner->GetWorld() : nullptr;
		if (ownerWorld)
		{
			RegisterInWorld(ownerWorld);
		}
	}

	void Component::UnregisterComponent()
	{
		if (!IsRegistered())
		{
			return;
		}
		RegisterTickFunctions(false);
		Unregister();
		m_WorldCached = nullptr;
	}

	void Component::Destroy()
	{
		if (m_bDying)
		{
			return;
		}
		m_bDying = true;
		if (m_bHasBegunPlay)
		{
			EndPlay();
		}
		if (m_bInitialized)
		{
			Uninitialize();
		}
		if (IsRegistered())
		{
			UnregisterComponent();
		}

		if (Entity* owner = GetOwner())
		{
			owner->RemoveRuntimeComponent(this);
			if (owner->GetRootComponent() == this)
			{
				owner->SetRootComponent(nullptr);
			}
		}
		OnDetroyed(false);
		MarkPendingKill();
	}

	void Component::OnCreated()
	{
		m_bCreated = true;
	}

	void Component::OnDetroyed(bool detroyInChain)
	{
		m_bCreated = false;
	}

	Level* Component::GetLevel() const
	{
		Entity* owner = GetOwner();
		return (owner ? owner->GetLevel() : GetCreatorAs<Level>());
	}

	Entity* Component::GetOwner() const
	{
		return m_OwnerCached;
		//return GetCreatorAs<Entity>();
	}

	bool Component::HasTag(StringID tag) const
	{
		return (tag != EStringID::None) && (std::find(Tags.begin(), Tags.end(), tag) != Tags.end());
	}

	void Component::SetActive(bool active)
	{
		if (m_bIsActive != active)
		{
			m_bIsActive = active;
			SetTickEnable(active);
			// Maybe Broadcast OnActivated
		}
	}

	void Component::StartDestroy()
	{
		if (m_bHasBegunPlay)
		{
			EndPlay();
		}
		if (m_bInitialized)
		{
			Uninitialize();
		}
		Unregister();
		if (m_bCreated)
		{
			OnDetroyed(true);
		}
		m_WorldCached = nullptr;

		if (Entity* owner = GetOwner())
		{
			owner->RemoveOwnedComponent(this);
		}
		Super::StartDestroy();
	}

	void Component::OnRegister()
	{
		m_bRegistered = true;

		Entity* owner = GetOwner();
		if (!m_WorldCached->IsGameWorld() || m_OwnerCached == nullptr || m_OwnerCached->IsInitialized())// Don't activate for editor world;
		{
			SetActive(true);
		}
	}
	void Component::OnUnregister()
	{
		m_bRegistered = false;
	}

	void Component::CreateRenderState()
	{
		VM_CORE_ASSERT(m_bRegistered);
		VM_CORE_ASSERT(!m_bRenderStateCreated);
		m_bRenderStateCreated = true;
		m_bRenderStateDirty = false;
		m_bRenderTransformDirty = false;
	}

	void Component::DestroyRenderState()
	{
		VM_CORE_ASSERT(m_bRenderStateCreated);
		m_bRenderStateCreated = false;
		m_bRenderStateDirty = false;
		m_bRenderTransformDirty = false;
	}

	void Component::CreatePhysicsState()
	{

	}

	void Component::DestroyPhysicsState()
	{

	}

	void Component::RegisterTickFunction(bool bRegister)
	{
		if (bRegister)
		{
			bool registered = false;
			if (DefaultComponentTick.bTickable && !IsPrototype())
			{
				Entity* owner = GetOwner();
				if (!owner || !owner->IsPrototype()) // May be remove prototype check
				{
					Level* level = (owner ? owner->GetLevel() : GetWorld()->CurrentLevel.Get());
					DefaultComponentTick.SetEnable(DefaultComponentTick.bTickOnStart || DefaultComponentTick.IsEnabled());
					DefaultComponentTick.Register(level);
					registered = true;
				}
			}
			if (registered)
			{
				DefaultComponentTick.TargetObject = this;
			}
		}
		else
		{
			if (DefaultComponentTick.IsRegistered())
			{
				DefaultComponentTick.Unregister();
			}
		}
	}

	World* Component::GetWorldInternal() const
	{
		World* owningWorld = nullptr;
		Entity* owner = GetOwner();
		if (owner)
		{
			owningWorld = owner->GetWorld();
		}
		return owningWorld;
	}

	void Component::Register()
	{
		if (!m_bRegistered)
		{
			OnRegister();
			VM_CORE_ASSERT(m_bRegistered, "Register attempt when registered flag is true");// Add name printing
		}
		// Render State
		// Physics State
	}

	void Component::Unregister()
	{
		// Destroy Physics
		// Destroy Render
		if (m_bRegistered)
		{
			OnUnregister();
			VM_CORE_ASSERT(m_bRegistered, "Unregister attempt when registered flag is false");// Add name printing
		}
	}

	void Component::RegisterProperties(ClassDescriptor* classDesc)
	{
		Object::RegisterProperties(classDesc);

		// Register:
		//		Tags
		//		TickFunction
		//		bActive
		//		bOnlyVisual
		// Find more what to register
	}
	void ComponentTickFunction::RunTick(float deltaTime)
	{
		if (TargetObject)
		{
			if (TargetObject->IsRegistered())
			{
				Entity* owner = TargetObject->GetOwner();
				deltaTime = owner ? owner->TimeScale : 1.0f;
				TargetObject->TickComponent(deltaTime);
			}
		}
	}

}

