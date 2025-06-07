#include "InputManager.h"

#include "Engine/InputComponent.h"

#include "Engine/Controller.h"
#include "Engine/Player.h"

#include <ranges>
namespace VeiM
{
	IMPLEMENT_CLASS(InputManager);

	struct DelegateDispatchInfo
	{
		uint32 EventIdx;
		uint32 FoundIdx;
		InputActionDelegate ADelegate;
		const InputActionBinding* Action;
		KeyWithMods Combo;
		EInputEvent::Type Event;

		DelegateDispatchInfo(
			const uint32 eventIdx,
			const size_t foundIdx,
			const KeyWithMods& combo,
			InputActionDelegate delegate,
			const EInputEvent::Type event,
			const InputActionBinding* action = nullptr)
			: EventIdx(eventIdx)
			, FoundIdx(foundIdx)
			, Combo(combo)
			, ADelegate(delegate)
			, Event(event)
			, Action(action)
		{
		}
	};


	void InputManager::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
	}

	InputManager::InputManager()
		: m_bMapsBuilt(false)
		, m_BuildIndex(0)
	{

	}

	void InputManager::AddActionMapping(const InputActionMapping& keyMapping)
	{
		ActionMappings.push_back(keyMapping);
		m_ActionMappingsMap.clear();
		m_bMapsBuilt = false;
	}

	void InputManager::RemoveActionMapping(const InputActionMapping& keyMapping)
	{
		for (int32 i = ActionMappings.size() - 1; i >= 0; i--)
		{
			if (ActionMappings[i] == keyMapping)
			{
				ActionMappings.erase(ActionMappings.begin() + i);
				m_ActionMappingsMap.clear();
				m_bMapsBuilt = false;
			}
		}
	}

	void InputManager::AddAxisMapping(const InputAxisMapping& keyMapping)
	{
		AxisMappings.push_back(keyMapping);
		m_AxisMappingsMap.clear();
		m_bMapsBuilt = false;
	}

	void InputManager::RemoveAxisMapping(const InputAxisMapping& keyMapping)
	{
		for (int32 i = AxisMappings.size() - 1; i >= 0; i--)
		{
			const InputAxisMapping& mapping = AxisMappings[i];
			if (mapping.Name == keyMapping.Name && mapping.AKey == keyMapping.AKey)
			{
				AxisMappings.erase(AxisMappings.begin() + i);
				m_AxisMappingsMap.clear();
				m_bMapsBuilt = false;
			}
		}
	}

	void InputManager::ProcessInputStack(const std::vector<InputComponent*>& inputStack, const float deltaTime)
	{
		RebuildMappingsMaps();

		static std::vector<std::pair<Key, KeyState*>> keysWithEvent;

		for (auto& [key, keyState] : m_KeyStateMap)
		{
			if (keyState.EventAccum.size() > 0)
			{
				keysWithEvent.emplace_back(key, &keyState);
			}
			keyState.EventAccumSave = keyState.EventAccum;
			keyState.EventAccum.clear();

			if (keyState.EventAccumSave.size() == 0 && key.AlwaysUpdateAxis())
			{
				keyState.Value = keyState.RawValue;
				keyState.EventAccumSave.push_back(EInputEvent::Released);
				keysWithEvent.emplace_back(key, &keyState);
			}
			ProcessKeys(key, &keyState);
			keyState.RawValue = { 0.0f, 0.0f };
		}
		ExecuteMappings(inputStack, deltaTime, keysWithEvent);

		for (auto& [key, keyState] : m_KeyStateMap)
		{
			keyState.bDownPrev = keyState.bDown;
			keyState.bConsumed = false;
		}

		keysWithEvent.clear();
	}

	void InputManager::ProcessKeys(Key key, KeyState* keyState)
	{
		// Process 2d axis with deadzones and exponent
		// Process mouseX/Y with fov scale
		if (key.IsAxis())
		{
			keyState->Value.x = keyState->RawValue.x;
		}

		auto it = std::find_if(keyState->EventAccumSave.rbegin(), keyState->EventAccumSave.rend(), [](EInputEvent::Type e)
			{
				return e == EInputEvent::Pressed || e == EInputEvent::Released;
			});
		if (it == keyState->EventAccumSave.rend())
		{
			keyState->bDown = keyState->bDownPrev;
		}
		else if (*it == EInputEvent::Pressed)
		{
			keyState->bDown = true;
		}
		else
		{
			keyState->bDown = false;
		}
	}

	void InputManager::RefreshPressedKeys()
	{
		Controller* controller = GetCreatorAs<Controller>();
		Player* player = controller ? controller->OwningPlayer : nullptr;
		if (player)
		{
			std::vector<Key> pressedKeys;

			for (const auto& it : m_KeyStateMap)
			{
				const KeyState& keyState = it.second;
				if (keyState.bDown)
				{
					pressedKeys.push_back(it.first);
				}
			}

			if (!pressedKeys.empty())
			{
				for (int32 i = 0; i < pressedKeys.size(); i++)
				{
					Key& key = pressedKeys[i];
					KeyParams params(key, EInputEvent::Released, {0.0f, 0.0f});
					InputKey(params);
				}
			}
		}
		for (auto& it : m_KeyStateMap)
		{
			KeyState& keyState = it.second;
			keyState.Value = { 0.0f, 0.0f };
			keyState.bDown = false;
			keyState.bDownPrev = false;
			keyState.bFresh = true;
		}
	}

	uint32 InputManager::GetKeyMapBuildIndex()
	{
		return m_BuildIndex;
	}

	const std::vector<VeiM::InputActionMapping>& InputManager::GetKeysForAction(const StringID name) const
	{
		RebuildMappingsMaps();
		if (const auto& it = m_ActionMappingsMap.find(name); it != m_ActionMappingsMap.end())
		{
			return it->second;
		}
		return InputManager::s_NoKeyMappings;
	}

	bool InputManager::InputKey(const KeyParams& params)
	{
		bool bGamepad = params.Keyp.IsGamepad();
		bool bAnalog = params.Keyp.IsAnalog() && (params.Keyp != IKey::MouseX && params.Keyp != IKey::MouseY);

		if (bAnalog)
		{
			auto CheckTransition = [this, &params](KeyState& keystate, float lastValue)
				{
					if (lastValue == 0.f && glm::length2(params.Offset) != 0.f)
					{
						keystate.EventAccum.push_back(EInputEvent::Pressed);
					}
					else if (lastValue != 0.f && glm::length2(params.Offset) == 0.f)
					{
						keystate.EventAccum.push_back(EInputEvent::Released);
					}
					else
					{
						keystate.EventAccum.push_back(EInputEvent::Repeat);
					}
				};

			KeyState& keyState = m_KeyStateMap[params.Keyp];
			CheckTransition(keyState, keyState.Value.x);
			keyState.RawValue = params.Offset;

			return false;
		}
		bool bNewEvent = m_KeyStateMap.count(params.Keyp) == 0;
		KeyState& keyState = m_KeyStateMap[params.Keyp];
		bNewEvent = bNewEvent || keyState.bFresh;

		if (bNewEvent && params.Event == EInputEvent::Repeat)
		{
			keyState.RawValue.x = params.Offset.x;
			keyState.EventAccum.push_back(EInputEvent::Pressed);
			keyState.bDown = true;
			keyState.bDownPrev = true;
		}

		switch (params.Event)
		{
		case EInputEvent::Pressed:
		case EInputEvent::Repeat:
			keyState.RawValue.x = params.Offset.x;
			keyState.EventAccum.push_back(params.Event);
			break;
		case EInputEvent::Released:
			keyState.RawValue.x = 0.f;
			keyState.EventAccum.push_back(EInputEvent::Released);
			break;
		case EInputEvent::DoubleClick:
			keyState.RawValue.x = params.Offset.x;
			keyState.EventAccum.push_back(EInputEvent::Pressed);
			keyState.EventAccum.push_back(EInputEvent::DoubleClick);
			break;
		}

		if (params.Event == EInputEvent::Pressed)
		{
			return IsKeyHandled(params.Keyp);
		}
		keyState.bFresh = false;
		return true;
	}

	bool InputManager::IsPressed(Key key)
	{
		if (key == IKey::AnyKey)
		{
			for (auto& [mapKey, mapKeyState] : m_KeyStateMap)
			{
				if (!mapKey.IsAnalog() && mapKeyState.bDown)
				{
					return true;
				}
			}
		}
		else if (auto it = m_KeyStateMap.find(key); it != m_KeyStateMap.end())
		{
			return it->second.bDown;
		}
		return false;
	}

	float InputManager::GetKeyValue(Key key) const
	{
		const auto& keyStateIt = m_KeyStateMap.find(key);
		if (keyStateIt != m_KeyStateMap.end())
		{
			return keyStateIt->second.Value.x;
		}
		return 0.0f;
	}

	bool InputManager::IsKeyHandled(Key key)
	{
		for (const InputActionMapping& mapping : ActionMappings)
		{
			if ((mapping.AKey == key || mapping.AKey == IKey::AnyKey) &&
				(mapping.bShift == false || IsShiftActive()) &&
				(mapping.bCtrl == false || IsCtrlActive()) &&
				(mapping.bAlt == false || IsAltActive()) &&
				(mapping.bSuper == false || IsSuperActive()))
			{
				return true;
			}
		}
		return false;
	}

	bool InputManager::IsShiftActive()
	{
		return IsPressed(IKey::LeftShift) || IsPressed(IKey::RightShift);
	}

	bool InputManager::IsCtrlActive()
	{
		return IsPressed(IKey::LeftControl) || IsPressed(IKey::RightControl);
	}

	bool InputManager::IsAltActive()
	{
		return IsPressed(IKey::LeftAlt) || IsPressed(IKey::RightAlt);
	}

	bool InputManager::IsSuperActive()
	{
		return IsPressed(IKey::LeftSuper) || IsPressed(IKey::RightSuper);
	}

	void InputManager::RebuildMappingsMaps() const
	{
		if (m_bMapsBuilt)
		{
			return;
		}
		if (m_ActionMappingsMap.empty())
		{
			for (const InputActionMapping& mapping : ActionMappings)
			{
				std::vector<InputActionMapping>& keyMappings = m_ActionMappingsMap[mapping.Name];
				keyMappings.push_back(mapping);
			}
			m_BuildIndex++;
		}
		if (m_AxisMappingsMap.empty())
		{
			for (const InputAxisMapping& mapping : AxisMappings)
			{
				bool bAdd = true;
				std::vector<InputAxisMapping>& keyMappings = m_AxisMappingsMap[mapping.Name];
				for (const InputAxisMapping& keyMapping : keyMappings)
				{
					if (keyMapping.AKey == mapping.AKey)
					{
						bAdd = false;
						break;
					}
				}
				if (bAdd)
				{
					keyMappings.push_back(mapping);
				}
			}
		}
		m_bMapsBuilt = true;
	}

	void InputManager::ExecuteMappings(const std::vector<InputComponent*>& inputStack, const float deltaTime, const std::vector<std::pair<Key, KeyState*>>& keysWithEvent)
	{
		struct AxisDelegateData
		{
			InputAxisDelegate Delegate;
			float Value;

			AxisDelegateData(InputAxisDelegate delegate, const float value)
				: Delegate(delegate)
				, Value(value)
			{
			}
		};
		// Add 2daxis delegate for gamepad

		static std::vector<AxisDelegateData> axisDelegates;
		static std::vector<DelegateDispatchInfo> nonAxisDelegates;
		static std::vector<Key> keysToConsume;// All gathered keys for events that are happening now and should be consumed
		static std::vector<DelegateDispatchInfo> foundKeysActivations; // Info for all actions that are activated now
		static std::vector<SharedPtr<InputActionBinding>> potentialActions;// Holds all bindings that bound to action that mapped to keys that have event now

		int32 index = inputStack.size() - 1;
		for (; index >= 0; index--)
		{
			InputComponent* const ic = inputStack[index];
			if (!ic || ic->HasFlag(Object::Flags::FLAG_PENDING_KILL))
			{
				continue;
			}

			ic->RebuildKeyMap(this);

			// Gather all action bindings for keys with events
			for (const std::pair<Key, KeyState*>& keyWithEvent : keysWithEvent)
			{
				if (!keyWithEvent.second->bConsumed)
				{
					ic->GetActionsBoundToKey(this, keyWithEvent.first, potentialActions);
				}
			}

			// for callbacks bound with BindAction for all potential actions
			// Gather all delegate dispatch infos from potential actions, if suits mods combo
			for (const SharedPtr<InputActionBinding>& actionBind : potentialActions)
			{
				GetKeyActivationsForAction(*actionBind.get(), foundKeysActivations, keysToConsume);
			}
			potentialActions.clear();

			// For callbacks bound with BindKey
			// Gather all delegate dispatch infos from key bindings, if suits mods combo
			for (const InputKeyBinding& keyBinding : ic->m_KeyBindings)
			{
				GetKeyActivationsForKey(keyBinding, foundKeysActivations, keysToConsume);
			}
			std::sort(foundKeysActivations.begin(), foundKeysActivations.end(), [](const DelegateDispatchInfo& a, const DelegateDispatchInfo& b)
				{
					return (a.EventIdx == b.EventIdx ? a.FoundIdx < b.FoundIdx : a.EventIdx < b.EventIdx);
				});

			// Now we gathered all delegates that should be called

			// Put all action and key delegates to nonAxisDelegates
			for (int32 i = 0; i < foundKeysActivations.size(); i++)
			{
				const DelegateDispatchInfo& foundKeyActivation = foundKeysActivations[i];
				if (foundKeysActivations[i].ADelegate.IsBound())
				{
					foundKeysActivations[i].FoundIdx = nonAxisDelegates.size();
					nonAxisDelegates.push_back(foundKeysActivations[i]);
				}

			}
			bool bHadAnyKeys = false;
			for (InputAxisBinding& ab : ic->m_AxisBindings)
			{
				ab.AxisValue = CalcAxisValue(ab, keysToConsume, bHadAnyKeys);
				if (bHadAnyKeys && ab.IADelegate.IsBound())
				{
					axisDelegates.emplace_back(AxisDelegateData(ab.IADelegate, ab.AxisValue));
				}
			}
			for (InputAxisKeyBinding& akb : ic->m_AxisKeyBindings)
			{
				if (!IsKeyConsumed(akb.AxisKey))
				{
					akb.AxisValue = GetKeyValue(akb.AxisKey);

					if (akb.bConsume)
					{
						keysToConsume.push_back(akb.AxisKey);
					}
				}
				else
				{
					akb.AxisValue = 0.0f;
				}
				if (akb.IADelegate.IsBound())
				{
					axisDelegates.emplace_back(AxisDelegateData(akb.IADelegate, akb.AxisValue));
				}

			}
			// TODO: Add loop for 2d axis bindings for gamepad

			if (ic->bBlockInput)
			{
				--index;
				keysToConsume.clear();
				foundKeysActivations.clear();
				break;
			}
			for (int32 i = 0; i < keysToConsume.size(); i++)
			{
				if (const auto& keyStateIt = m_KeyStateMap.find(keysToConsume[i]); keyStateIt != m_KeyStateMap.end())
				{
					keyStateIt->second.bConsumed = true;
				}
			}
			keysToConsume.clear();
			foundKeysActivations.clear();

		}
		for (; index >= 0; index--)
		{
			InputComponent* ic = inputStack[index];
			if (!ic)
			{
				continue;
			}
			for (InputAxisBinding& ab : ic->m_AxisBindings)
			{
				ab.AxisValue = 0.0f;
			}
			for (InputAxisKeyBinding& akb : ic->m_AxisKeyBindings)
			{
				akb.AxisValue = 0.0f;
			}
		}
		// Dispatch
		std::sort(nonAxisDelegates.begin(), nonAxisDelegates.end(), [](const DelegateDispatchInfo& a, const DelegateDispatchInfo& b)
			{
				return (a.EventIdx == b.EventIdx ? a.FoundIdx < b.FoundIdx : a.EventIdx < b.EventIdx);
			});
		for (const DelegateDispatchInfo& info : nonAxisDelegates)
		{
			if (info.ADelegate.IsBound())
			{
				info.ADelegate.Execute();
			}
		}
		for (const AxisDelegateData& dat : axisDelegates)
		{
			if (dat.Delegate.IsBound())
			{
				dat.Delegate.Execute(dat.Value);
			}
		}
		axisDelegates.clear();
		nonAxisDelegates.clear();

	}

	void InputManager::GetKeyActivationsForAction(const InputActionBinding& actionBinding, std::vector<struct DelegateDispatchInfo>& foundKeys, std::vector<Key>& keysToHandle)
	{
		RebuildMappingsMaps();

		auto mappingsIt = m_ActionMappingsMap.find(actionBinding.GetActionName());
		if (mappingsIt == m_ActionMappingsMap.end())
		{
			return;
		}
		// For each action mapping get mapped key, and get dispatch info
		for (const InputActionMapping& keyMapping : mappingsIt->second)
		{
			if (keyMapping.AKey == IKey::AnyKey)
			{
				for (auto& keyStateIt : m_KeyStateMap)
				{
					const Key& key = keyStateIt.first;
					if (!key.IsAnalog() && !keyStateIt.second.bConsumed)
					{
						InputActionMapping subKeyMapping(keyMapping);
						subKeyMapping.AKey = key;
						GetKeyActivationsForMapping(subKeyMapping, actionBinding, foundKeys, keysToHandle, &keyStateIt.second);
					}
				}
			}
			else
			{
				auto keyStateIt = m_KeyStateMap.find(keyMapping.AKey);
				if (keyStateIt != m_KeyStateMap.end() && !IsKeyConsumed(keyMapping.AKey, &keyStateIt->second))
				{
					GetKeyActivationsForMapping(keyMapping, actionBinding, foundKeys, keysToHandle, &keyStateIt->second);
				}
			}
		}
	}

	void InputManager::GetKeyActivationsForMapping(const InputActionMapping& mapping, const InputActionBinding& actionBinding, std::vector<DelegateDispatchInfo>& foundKeys, std::vector<Key>& keysToConsume, const KeyState* keyState)
	{
		bool bConsume = false;

		if ((mapping.bShift == false || IsShiftActive()) &&
			(mapping.bCtrl == false || IsCtrlActive()) &&
			(mapping.bAlt == false || IsAltActive()) &&
			(mapping.bSuper == false || IsSuperActive()) &&
			KeyEventJustHappened(mapping.AKey, actionBinding.KeyEvent, m_EventIndices, keyState)) // Return true if given KeyState has the passed event right now
		{
			bool bAddDelegate = true;
			// Combo for current mapping
			const KeyWithMods combo(mapping.AKey, mapping.bShift, mapping.bCtrl, mapping.bAlt, mapping.bSuper);

			for (int32 i = foundKeys.size() - 1; i >= 0; i--)
			{
				if (combo.Masks(foundKeys[i].Combo))
				{
					foundKeys.erase(foundKeys.begin() + i);
				}
				else if (foundKeys[i].Combo.Masks(combo))
				{
					bAddDelegate = false;
					break;
				}
			}

			// Add combo to foundKeys from current checking mapping if not masked by already added 
			if (bAddDelegate)
			{
				DelegateDispatchInfo foundKey(m_EventIndices[0], foundKeys.size(), combo
					, actionBinding.IADelegate, actionBinding.KeyEvent, &actionBinding);

				const int32 lastEventIdx = m_EventIndices.size() - 1;
				for (int32 i = 0; i <= lastEventIdx; i++)
				{
					foundKey.EventIdx = m_EventIndices[i];
					foundKeys.emplace_back(foundKey);
				}
				bConsume = true;
			}

		}
		// Add key if no modifiers needed or they are satisfied
		if (actionBinding.bConsume && (bConsume || !(mapping.bShift || mapping.bCtrl || mapping.bAlt || mapping.bSuper || actionBinding.KeyEvent == EInputEvent::DoubleClick)))
		{
			keysToConsume.push_back(mapping.AKey);
		}
		m_EventIndices.clear();
	}

	void InputManager::GetKeyActivationsForKey(const InputKeyBinding& keyBinding, std::vector<DelegateDispatchInfo>& foundKeys, std::vector<Key>& keysToConsume, const KeyState* keyState /*= nullptr*/)
	{
		bool bConsume = false;
		if (keyBinding.InputKeys.Keyp == IKey::AnyKey)
		{
			for (const auto& keyStateIt : m_KeyStateMap)
			{
				const Key& key = keyStateIt.first;
				if (!key.IsAnalog() && !keyStateIt.second.bConsumed)
				{
					InputKeyBinding subKeyBinding(keyBinding);
					subKeyBinding.InputKeys.Keyp = key;
					GetKeyActivationsForKey(subKeyBinding, foundKeys, keysToConsume, &keyStateIt.second);
				}
			}
		}
		else
		{
			if (!keyState)
			{
				const auto& keyStateIt = m_KeyStateMap.find(keyBinding.InputKeys.Keyp);
				if (keyStateIt != m_KeyStateMap.end())
				{
					keyState = &keyStateIt->second;
				}
			}
			if (!IsKeyConsumed(keyBinding.InputKeys.Keyp, keyState))
			{
				if ((keyBinding.InputKeys.bShift == false || IsShiftActive()) &&
					(keyBinding.InputKeys.bCtrl == false || IsCtrlActive()) &&
					(keyBinding.InputKeys.bAlt == false || IsAltActive()) &&
					(keyBinding.InputKeys.bSuper == false || IsSuperActive()) &&
					KeyEventJustHappened(keyBinding.InputKeys.Keyp, keyBinding.Event, m_EventIndices, keyState)
					)
				{
					bool bAddDelegate = true;

					for (int32 i = foundKeys.size() - 1; i >= 0; i--)
					{
						if (keyBinding.InputKeys.Masks(foundKeys[i].Combo))
						{
							foundKeys.erase(foundKeys.begin() + i);
						}
						else if (foundKeys[i].Combo.Masks(keyBinding.InputKeys))
						{
							bAddDelegate = false;
							break;
						}
					}
					if (bAddDelegate)
					{
						DelegateDispatchInfo foundKey(m_EventIndices[0], foundKeys.size(), keyBinding.InputKeys
							, keyBinding.Delegate, keyBinding.Event);

						const int32 lastEventIdx = m_EventIndices.size() - 1;
						for (int32 i = 0; i <= lastEventIdx; i++)
						{
							foundKey.EventIdx = m_EventIndices[i];
							foundKeys.emplace_back(foundKey);
						}
						bConsume = true;
					}
					m_EventIndices.clear();

				}
			}
		}
		if (keyBinding.bConsume && (bConsume || !(keyBinding.InputKeys.bShift || keyBinding.InputKeys.bCtrl || keyBinding.InputKeys.bAlt || keyBinding.InputKeys.bSuper || keyBinding.Event == EInputEvent::DoubleClick)))
		{
			keysToConsume.push_back(keyBinding.InputKeys.Keyp);
		}
	}

	float InputManager::CalcAxisValue(const InputAxisBinding& axisBinding, std::vector<Key>& keysToConsume, bool& bHadNotConsumed) const
	{
		RebuildMappingsMaps();
		float axisValue = 0.0f;
		bHadNotConsumed = false;

		const auto& mappingsIt = m_AxisMappingsMap.find(axisBinding.AxisName);
		if (mappingsIt == m_AxisMappingsMap.end())
		{
			return axisValue;
		}
		const std::vector<InputAxisMapping>& mappings = mappingsIt->second;
		for (int32 i = 0; i < mappings.size(); i++)
		{
			const InputAxisMapping& keyMapping = mappings[i];
			if (!IsKeyConsumed(keyMapping.AKey))
			{
				axisValue += GetKeyValue(keyMapping.AKey) * keyMapping.Scale;
			}
			if (axisBinding.bConsume)
			{
				keysToConsume.push_back(keyMapping.AKey);
			}
			bHadNotConsumed = true;
		}
		return axisValue;
	}

	bool InputManager::KeyEventJustHappened(Key key, EInputEvent::Type event, std::vector<uint32>& eventIndices, const KeyState* keyState /*= nullptr*/) const
	{
		if (!keyState)
		{
			const auto& keyStateIt = m_KeyStateMap.find(key);
			keyState = (keyStateIt != m_KeyStateMap.end()) ? &keyStateIt->second : nullptr;
		}
		if (keyState && std::find(keyState->EventAccumSave.begin(), keyState->EventAccumSave.end(), event) != keyState->EventAccumSave.end())
		{
			for (size_t i = 0; auto v : keyState->EventAccumSave)
			{
				if (v == event) eventIndices.push_back(i);
				++i;
			}
			return true;
		}
		return false;
	}

	bool InputManager::IsKeyConsumed(Key key, const KeyState* keyState /*= nullptr*/) const
	{
		if (key == IKey::AnyKey)
		{
			for (const auto& keyStateIt : m_KeyStateMap)
			{
				if (keyStateIt.second.bConsumed)
				{
					return true;
				}
			}
		}
		else
		{
			if (!keyState)
			{
				const auto& keyStateIt = m_KeyStateMap.find(key);
				if (keyStateIt != m_KeyStateMap.end())
				{
					keyState = &keyStateIt->second;
				}
				if (keyState)
				{
					return keyState->bConsumed;
				}
			}
		}
		return false;
	}

	const std::vector<VeiM::InputActionMapping> InputManager::s_NoKeyMappings;

}