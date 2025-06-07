#pragma once
#include "CoreDefines.h"
#include "Engine/CoreObject.h"
#include "Engine/Reflection.h"
#include "Input/Input.h"
#include "Engine/EngineTypes.h"

#include "Types/StringID.h"
#include "Utils/Math.h"
#include <glm/glm.hpp>

namespace VeiM
{
	struct InputActionBinding;
	struct InputKeyBinding;
	struct InputAxisBinding;

	struct KeyParams
	{
		KeyParams() = default;
		KeyParams(Key key, EInputEvent::Type event, glm::vec2 offset)
			: Keyp(key)
			, Event(event)
			, Offset(offset)
		{
		}
		KeyParams(Key key, float offset)
			: Keyp(key)
			, Offset({ offset, 0.0f})
		{
		}

		float GetOffset1D() const { return Offset.x; }
		glm::vec2 GetOffset2D() const { return Offset; }


		Key Keyp = IKey::Invalid;
		EInputEvent::Type Event = EInputEvent::Pressed;
		glm::vec2 Offset = Math::Utils::ZeroVector2d;
	};

	struct InputActionMapping
	{
		Key AKey;
		StringID Name;
		uint8 bShift : 1;
		uint8 bCtrl : 1;
		uint8 bAlt : 1;
		uint8 bSuper : 1;

		InputActionMapping(
			const StringID name = EStringID::None,
			const Key key = IKey::Invalid,
			const bool bshift = false,
			const bool bctrl = false,
			const bool balt = false,
			const bool bsuper = false)
			: Name(name)
			, bShift(bshift)
			, bCtrl(bctrl)
			, bAlt(balt)
			, bSuper(bsuper)
			, AKey(key)
		{
		}
		bool operator==(const InputActionMapping& other) const
		{
			return (
				Name == other.Name &&
				AKey == other.AKey &&
				bShift == other.bShift &&
				bCtrl == other.bCtrl &&
				bAlt == other.bAlt &&
				bSuper == other.bSuper);
		}
	};

	struct InputAxisMapping
	{
		Key AKey;
		StringID Name;
		float Scale;

		InputAxisMapping(
			const StringID name = EStringID::None,
			const Key key = IKey::Invalid,
			const float scale = 1.f)
			: Name(name)
			, AKey(key)
			, Scale(scale)
		{
		}
	};

	class InputComponent;

	class CORE_API InputManager : public Object
	{
		DECLARE_CLASS(InputManager, Object)
	public:
		InputManager();

		void AddActionMapping(const InputActionMapping& keyMapping);
		void RemoveActionMapping(const InputActionMapping& keyMapping);
		void AddAxisMapping(const InputAxisMapping& keyMapping);
		void RemoveAxisMapping(const InputAxisMapping& keyMapping);

		void ProcessInputStack(const std::vector<InputComponent*>& inputStack, const float deltaTime);
		void ProcessKeys(Key key, KeyState* keyState);

		void RefreshPressedKeys();
		bool InputKey(const KeyParams& params);

		bool IsPressed(Key key);
		float GetKeyValue(Key key) const;
		bool IsKeyHandled(Key key);
		bool IsShiftActive();
		bool IsCtrlActive();
		bool IsAltActive();
		bool IsSuperActive();

		uint32 GetKeyMapBuildIndex();
		const std::vector<InputActionMapping>& GetKeysForAction(const StringID name) const;
	private:
		void RebuildMappingsMaps() const;
		void ExecuteMappings(const std::vector<InputComponent*>& inputStack, const float deltaTime, const std::vector<std::pair<Key, KeyState*>>& keysWithEvent);
		void GetKeyActivationsForAction(const InputActionBinding& actionBinding, std::vector<struct DelegateDispatchInfo>& foundKeys, std::vector<Key>& keysToHandle);
		void GetKeyActivationsForMapping(const InputActionMapping& mapping, const InputActionBinding& actionBinding, std::vector<DelegateDispatchInfo>& foundKeys, std::vector<Key>& keysToHandle, const KeyState* keyState);
		void GetKeyActivationsForKey(const InputKeyBinding& keyBinding, std::vector<DelegateDispatchInfo>& foundKeys, std::vector<Key>& keysToHandle, const KeyState* keyState = nullptr);
		float CalcAxisValue(const InputAxisBinding& axisBinding, std::vector<Key>& keysToConsume, bool& bHadNotConsumed) const;

		bool KeyEventJustHappened(Key key, EInputEvent::Type event, std::vector<uint32>& eventIndices, const KeyState* keyState = nullptr) const;
		bool IsKeyConsumed(Key key, const KeyState* keyState = nullptr) const;
	public:
		std::vector<InputActionMapping> ActionMappings;
		std::vector<InputAxisMapping> AxisMappings;

	private:
		std::unordered_map<Key, KeyState> m_KeyStateMap;

		mutable std::unordered_map<StringID, std::vector<InputActionMapping>> m_ActionMappingsMap;
		mutable std::unordered_map<StringID, std::vector<InputAxisMapping>> m_AxisMappingsMap;
		mutable bool m_bMapsBuilt;
		mutable uint32 m_BuildIndex;
		std::vector<uint32> m_EventIndices;
		static const std::vector<InputActionMapping> s_NoKeyMappings;
	};
}