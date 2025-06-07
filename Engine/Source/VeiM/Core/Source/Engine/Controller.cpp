#include "Controller.h"

#include "Engine/InputComponent.h"
#include "Engine/Player.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/World.h"
#include "Engine/Level.h"

namespace VeiM
{
	IMPLEMENT_CLASS(Controller);


	void Controller::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(Controller, ObjectProperty, PlayerInputManager);
		REGISTER_PROPERTY(Controller, ObjectProperty, OwningPlayer);
		REGISTER_PROPERTY(Controller, ObjectProperty, m_Agent);
	}

	Controller::Controller()
	{
		DefaultEntityTick.bTickable = true;
		SetHiddenInGame(true);
		m_bInputEnabled = true;
#ifdef VM_WITH_EDITOR
		bHiddenInEditor = true;
#endif
	}

	void Controller::PushInputComponent(InputComponent* inputComponent)
	{
		if (!inputComponent)
		{
			return;
		}
		bool bPushed = false;
		m_ClientInputStack.erase(std::remove(m_ClientInputStack.begin(), m_ClientInputStack.end(), WeakObjectPtr(inputComponent)), m_ClientInputStack.end());
		for (int32 i = m_ClientInputStack.size() - 1; i >= 0; i--)
		{
			InputComponent* icomp = m_ClientInputStack[i].Get();
			if (!icomp)
			{
				m_ClientInputStack.erase(m_ClientInputStack.begin() + i);
			}
			else if (icomp->Priority <= inputComponent->Priority)
			{
				m_ClientInputStack.insert(m_ClientInputStack.begin() + i + 1, WeakObjectPtr(inputComponent));
				bPushed = true;
				break;
			}
		}
		if (!bPushed)
		{
			m_ClientInputStack.insert(m_ClientInputStack.begin(), WeakObjectPtr(inputComponent));
		}
	}

	bool Controller::PopInputComponent(InputComponent* inputComponent)
	{
		if (!inputComponent)
		{
			return false;
		}
		size_t sizeBefore = m_ClientInputStack.size();
		m_ClientInputStack.erase(std::remove(m_ClientInputStack.begin(), m_ClientInputStack.end(), WeakObjectPtr(inputComponent)), m_ClientInputStack.end());

		if (sizeBefore != m_ClientInputStack.size());
		{
			inputComponent->ClearBindings();
			return true;
		}
		return false;

	}

	void Controller::Posses(Agent* agent)
	{
		if (agent != GetAgent() && GetAgent())
		{
			UnPossess();
		}
		if (!agent)
		{
			return;
		}
		if (agent->PossessingController)
		{
			agent->PossessingController->UnPossess();
		}
		agent->PossessWith(this);
		m_Agent = agent;
		if (GetAgent()->DefaultEntityTick.bTickOnStart)
		{
			GetAgent()->SetTickEnable(true);
		}

		agent->RestartControl();

		InState = StringID("Playing");

	}

	void Controller::UnPossess()
	{
		if (!m_Agent)
		{
			return;
		}

		if (m_Agent)
		{
			m_Agent->OnUnPosses();
			m_Agent = nullptr;
		}
	}

	void Controller::InitInput()
	{
		if (!PlayerInputManager)
		{
			PlayerInputManager = NewObject<InputManager>(this, StringID((GetName() + "InputManager").data()));
		}
		SetupInputComponent();

		GetWorld()->CurrentLevel->ProcessNewInputInitWithInputList(this);

	}

	void Controller::SetPlayer(Player* player)
	{
		OwningPlayer = player;
		player->PlayerController = this;
		InitInput();
	}

	void Controller::OwnedAgentDestroy(Agent* agent)
	{
		if (InState == StringID("Inactive"))
		{
			VM_TRACE("OnOWsnedAgnetDestroy called for inactive Controller");
		}
		if (agent != m_Agent)
		{
			return;
		}
		UnPossess();
		InState = StringID("Inactive");
		Destroy();
	}

	void Controller::TickInWorld(float deltaTime)
	{
		if (!PlayerInputManager && OwningPlayer == nullptr)
		{
			InitInput();
		}

		if (PlayerInputManager)
		{
			ProcessInput(deltaTime);
			// TODO: Update rotation (deltaTime)
		}
		if (HasFlag(Flags::FLAG_PENDING_KILL))
		{
			return;
		}

		Tick(deltaTime);

	}

	bool Controller::InputKey(const KeyParams& params)
	{
		//TODO: Check for input device user is the same as user of player that has this controller
		if (PlayerInputManager)
		{
			return PlayerInputManager->InputKey(params);
		}

		return false;
	}

	void Controller::ProcessInput(const float deltaTime)
	{
		static std::vector<InputComponent*> inputStack;

		BuildInputStack(inputStack);
		PlayerInputManager->ProcessInputStack(inputStack, deltaTime);
		inputStack.clear();
	}

	void Controller::BuildInputStack(std::vector<InputComponent*>& inputStack)
	{
		Agent* controlledAgent = GetAgent();
		if (controlledAgent && controlledAgent->InputEnabled() && controlledAgent->EntityInputComponent)
		{
			inputStack.push_back(controlledAgent->EntityInputComponent);
		}
		if (InputEnabled())
		{
			inputStack.push_back(EntityInputComponent);
		}
		for (int32 i = 0; i < m_ClientInputStack.size(); i++)
		{
			InputComponent* ic = m_ClientInputStack[i].Get();
			if (!ic->HasFlag(FLAG_PENDING_KILL))
			{
				inputStack.push_back(ic);
			}
			else
			{
				m_ClientInputStack.erase(m_ClientInputStack.begin() + i);
				--i;
			}
		}
	}

	void Controller::SetupInputComponent()
	{
		if (!EntityInputComponent)
		{
			EntityInputComponent = NewObject<InputComponent>(this, StringID("ControllerInputComponent"));
			EntityInputComponent->RegisterComponent();
		}
	}

}