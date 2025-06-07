#pragma once
#include "Engine/Entity.h"

namespace VeiM
{
	class CORE_API Agent : public Entity
	{
		DECLARE_CLASS(Agent, Entity)
	public:
		Agent();
		bool IsControlled();
		FORCEINLINE Controller* GetController() { return PossessingController; }
		void PossessWith(Controller* controller);
		void OnUnPosses();
		void RestartControl();

		virtual void SetupInputComponent(InputComponent* ic) {}

		bool InputEnabled() const { return m_bInputEnabled; }
		/* Object Interface */
		virtual void EndPlay() override;
		virtual void PreInitializeComponents() override;
		virtual void EnableInput(Controller* controller) override;
		virtual void DisableInput(Controller* controller) override;

	protected:
		InputComponent* CreateInputComponent();

	public:
		ObjectPtr<Controller> PossessingController;

		uint8 bUseControllerRotationPitch : 1;
		uint8 bUseControllerRotationYaw : 1;
		uint8 bUseControllerRotationRoll : 1;

	private:
		uint8 m_bInputEnabled : 1;
	};
}