#include "InputComponent.h"

#include "Engine/Classes/Game/InputManager.h"
#include "Engine/Entity.h"

namespace VeiM
{

	bool KeyWithMods::Masks(const KeyWithMods& rhs) const
	{
		if (Keyp != rhs.Keyp)
		{
			return false;
		}
		if (Equal(rhs))
		{
			return false;
		}
		else if (
			(bShift || !rhs.bShift)	&&
			(bCtrl	|| !rhs.bCtrl)	&&
			(bAlt	|| !rhs.bAlt)	&&
			(bSuper || !rhs.bSuper))
		{
			return true;
		}
	}

	bool KeyWithMods::Equal(const KeyWithMods& rhs) const
	{
		if (Keyp != rhs.Keyp)
		{
			return false;
		}

		if ((bShift == rhs.bShift) &&
			(bCtrl == rhs.bCtrl) &&
			(bAlt == rhs.bAlt) &&
			(bSuper == rhs.bSuper))
		{
			return true;
		}
		return false;
	}

	IMPLEMENT_CLASS(InputComponent);


	void InputActionBinding::GenNewHandle()
	{
		static int32 iabHandle = 1;
		m_Handle = iabHandle++;
	}



	void InputComponent::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		// TODO: Reigster
	}

	void InputComponent::ClearBindings()
	{
		for (InputAxisBinding& ab : m_AxisBindings)
		{
			ab.AxisValue = 0.0f;
		}
	}

	void InputComponent::RebuildKeyMap(InputManager* inputManager)
	{
		if (!inputManager)
		{
			return;
		}
		KeyToActionInfo* info = nullptr;

		for (int32 i = m_KeyToAction.size() - 1; i >= 0; i--)
		{
			info = &m_KeyToAction[i];
			if (info->Input == inputManager)
			{
				if (info->BuildIndex == inputManager->GetKeyMapBuildIndex())
				{
					return;
				}
				break;
			}
			else if (info->Input == nullptr)
			{
				m_KeyToAction.erase(m_KeyToAction.begin() + i);
			}
			info = nullptr;
		}
		if (info == nullptr)
		{
			m_KeyToAction.push_back(KeyToActionInfo());
			info = &m_KeyToAction.back();
			info->Input = inputManager;
			if (Entity* owner = inputManager->GetCreatorAs<Entity>())
			{
				owner->OnEndPlay.AddObject(this, &InputComponent::OnOwnerEndPlay);
			}
		}
		
		for (auto& keyBindPair : info->KeyToActionMap)
		{
			keyBindPair.second.clear();
		}
		info->AnyKeyToActionMap.clear();

		/* 
		* For each Binding get Action. For that Action get each key - action mapping 
		* Map Key to Bindings
		*/
		for (const SharedPtr<InputActionBinding>& actionBind : m_ActionBindings)
		{
			const std::vector<InputActionMapping>& keysForAction = inputManager->GetKeysForAction(actionBind->m_ActionName);
			for (const InputActionMapping& keyMapping : keysForAction)
			{
				if (keyMapping.AKey != IKey::AnyKey)
				{
					info->KeyToActionMap[keyMapping.AKey].push_back(actionBind);
				}
				else
				{
					info->AnyKeyToActionMap.push_back(actionBind);
				}

			}
		}
		info->BuildIndex = inputManager->GetKeyMapBuildIndex();
	}


	void InputComponent::OnOwnerEndPlay(Entity* entity)
	{
		for (int32 i = m_KeyToAction.size() - 1; i >= 0; i--)
		{
			KeyToActionInfo& info = m_KeyToAction[i];
			const InputManager* input = info.Input.Get();
			if (input && input->GetCreatorAs<Entity>() == entity)
			{
				m_KeyToAction.erase(m_KeyToAction.begin() + i);
			}
		}
	}

	void InputComponent::GetActionsBoundToKey(InputManager* inputManager, Key key, std::vector<SharedPtr<InputActionBinding>>& actions) const
	{
		// Get all bindings for key

		for (const KeyToActionInfo& info : m_KeyToAction)
		{
			if (info.Input == inputManager)
			{
				if (const auto& actionsForKeyIt = info.KeyToActionMap.find(key); actionsForKeyIt != info.KeyToActionMap.end())
				{
					for (const SharedPtr<InputActionBinding>& actionForKey : actionsForKeyIt->second)
					{
						actions.push_back(actionForKey);
					}
				}
				for (const SharedPtr<InputActionBinding>& actionForKey : info.AnyKeyToActionMap)
				{
					actions.push_back(actionForKey);
				}
				return;
			}
		}
	}

}