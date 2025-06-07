#include "Agent.h"
#include "Engine/Controller.h"
#include "Engine/InputComponent.h"
#include "Engine/GameStatics.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/World.h"
#include "Engine/Level.h"

namespace VeiM
{
	IMPLEMENT_CLASS(Agent);

	void Agent::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);

	}

	Agent::Agent()
	{
		DefaultEntityTick.bTickable = true;
		
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
		
		m_bInputEnabled = true;
	}

	bool Agent::IsControlled()
	{
		return PossessingController != nullptr;
	}


	void Agent::PossessWith(Controller* controller)
	{
		PossessingController = controller;

	}

	void Agent::OnUnPosses()
	{
		PossessingController = nullptr;
		if (EntityInputComponent)
		{
			EntityInputComponent->Destroy();
			EntityInputComponent = nullptr;
		}
	}

	void Agent::RestartControl()
	{
		if (!PossessingController)
		{
			return;
		}
		if (!EntityInputComponent)
		{
			EntityInputComponent = CreateInputComponent();
			SetupInputComponent(EntityInputComponent);
			EntityInputComponent->RegisterComponent();
		}
	}

	void Agent::EndPlay()
	{
		if (PossessingController && PossessingController->GetAgent() == this)
		{
			PossessingController->OwnedAgentDestroy(this);
			if (PossessingController)
			{
				PossessingController->UnPossess();
				PossessingController = nullptr;
			}
		}

		Super::EndPlay();
	}

	void Agent::PreInitializeComponents()
	{
		Super::PreInitializeComponents();

		if (ControllerIndex >= 0)
		{
			Controller* controller = GameStatics::GetController(ControllerIndex);
			if (controller)
			{
				controller->Posses(this);
			}
			else
			{
				GetWorld()->CurrentLevel->AddEntityToInputList(this, ControllerIndex);
			}
		}
	}

	void Agent::EnableInput(Controller* controller)
	{
		if (controller == PossessingController)
		{
			m_bInputEnabled = true;
		}
		else
		{
			VM_CORE_ERROR("EnableInput called on agent passing foreing controller");
		}
	}

	void Agent::DisableInput(Controller* controller)
	{
		if (controller == PossessingController)
		{
			m_bInputEnabled = false;
		}
		else
		{
			VM_CORE_ERROR("DisableInput called on agent passing foreign controller");
		}
	}

	InputComponent* Agent::CreateInputComponent()
	{
		return NewObject<InputComponent>(this, StringID((GetName() + "InputComponent").data()));
	}

}

