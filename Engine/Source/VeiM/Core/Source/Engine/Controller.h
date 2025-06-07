#pragma once
#include "Engine/Entity.h"
#include "Engine/Classes/Game/Agent.h"
#include "Engine/Classes/Game/InputManager.h"
#include "Input/Input.h"

namespace VeiM
{
	class InputComponent;
	class Player;

	class CORE_API Controller : public Entity
	{
		DECLARE_CLASS(Controller, Entity)
	public:
		Controller();

		virtual void PushInputComponent(InputComponent* inputComponent);
		virtual bool PopInputComponent(InputComponent* inputComponent);
		virtual void Posses(Agent* agent);
		virtual void UnPossess();
		virtual void InitInput();

		bool InputEnabled() const { return m_bInputEnabled; }
		void EnableInput(bool enabled) { m_bInputEnabled = enabled; }
		void DisableInput(bool enabled) { m_bInputEnabled = enabled; }


		void SetPlayer(Player* player);

		FORCEINLINE Agent* GetAgent() { return m_Agent; }
		void OwnedAgentDestroy(Agent* agent);


		/* Entity Interface */
		virtual void TickInWorld(float dletaTime) override;

		/* Input Handling */
		virtual bool InputKey(const KeyParams& params);

	protected:
		void ProcessInput(const float deltaTime);
		virtual void BuildInputStack(std::vector<InputComponent*>& inputStack);
		virtual void SetupInputComponent();

	public:
		StringID InState;
		ObjectPtr<Player> OwningPlayer;
		ObjectPtr<InputManager> PlayerInputManager;
	protected:
		std::vector<WeakObjectPtr<InputComponent>> m_ClientInputStack;

	private:
		ObjectPtr<Agent> m_Agent;
		bool m_bInputEnabled;
	};
}