#pragma once
#include "CoreDefines.h"
#include "Engine/Component.h"
#include "Types/Delegate.h"
#include "Input/Input.h"
#include "Engine/Classes/Game/InputManager.h"

namespace VeiM
{
	DECLARE_DELEGATE(InputActionDelegate);
	DECLARE_DELEGATE_1p(InputAxisDelegate, float);


	struct CORE_API  KeyWithMods
	{
		Key Keyp;
		bool bShift;
		bool bCtrl;
		bool bAlt;
		bool bSuper;

		KeyWithMods()
			: Keyp(IKey::Invalid)
			, bShift(false)
			, bCtrl(false)
			, bAlt(false)
			, bSuper(false)
		{ }

		KeyWithMods(const Key key)
			: Keyp(key)
			, bShift(false)
			, bCtrl(false)
			, bAlt(false)
			, bSuper(false)
		{
		}
		KeyWithMods(const Key key, const bool sh, const bool ct, const bool al, const bool su)
			: Keyp(key)
			, bShift(sh)
			, bCtrl(ct)
			, bAlt(al)
			, bSuper(su)
		{
		}

		bool Masks(const KeyWithMods&) const;
		bool Equal(const KeyWithMods&) const;
	};

	struct CORE_API  InputBinding
	{
		InputBinding() : bConsume(true), bRunOnPause(false) {}

		bool bConsume;
		bool bRunOnPause;
	};


	struct CORE_API  InputAxisBinding : public InputBinding
	{
		InputAxisBinding() : InputBinding(), AxisName(StringID()), AxisValue(0.f) {}
		InputAxisBinding(const StringID name) : InputBinding(), AxisName(name), AxisValue(0.f) {}

		StringID AxisName;
		InputAxisDelegate IADelegate;
		float AxisValue;

	};

	struct CORE_API InputActionBinding : public InputBinding
	{
	public:
		InputActionBinding() : InputBinding(), KeyEvent(EInputEvent::Pressed), m_ActionName(StringID()), m_Handle(-1) {}
		InputActionBinding(const StringID actionName, const EInputEvent::Type keyEvent) : InputBinding(), KeyEvent(keyEvent), m_ActionName(actionName), m_Handle(-1) {}
		StringID GetActionName() const { return m_ActionName; }
		int32 GetHandle() const { return m_Handle; }
		bool IsValid() { return m_Handle != -1; }
		void GenNewHandle();

		bool operator==(const InputActionBinding& other) { return (IsValid() && GetHandle() == other.GetHandle()); }

	public:
		EInputEvent::Type KeyEvent;
		InputActionDelegate IADelegate;

	private:
		StringID m_ActionName;
		int32 m_Handle;

		friend class InputComponent;
	};

	struct CORE_API  InputKeyBinding : public InputBinding
	{
		EInputEvent::Type Event;
		KeyWithMods InputKeys;
		InputActionDelegate Delegate;

		InputKeyBinding()
			: InputBinding()
			, Event(EInputEvent::Pressed)
		{
		}
		InputKeyBinding(const KeyWithMods keys, const EInputEvent::Type event)
			: InputBinding()
			, Event(event)
			, InputKeys(keys)
		{
		}
	};

	struct CORE_API  InputAxisKeyBinding : public InputBinding
	{
		InputAxisKeyBinding() : InputBinding(), AxisValue(0.0f) {}
		InputAxisKeyBinding(const Key key) : InputBinding(), AxisValue(0.0f), AxisKey(key) {}

		float AxisValue;
		Key AxisKey;
		InputAxisDelegate IADelegate;
	};

	struct CORE_API  KeyToActionInfo
	{
		WeakObjectPtr<InputManager> Input;
		uint32 BuildIndex;
		std::unordered_map<Key, std::vector<SharedPtr<InputActionBinding>>> KeyToActionMap;
		std::vector<SharedPtr<InputActionBinding>> AnyKeyToActionMap;

		KeyToActionInfo() : Input(nullptr), BuildIndex(0) {}
	};




	class CORE_API InputComponent : public Component
	{
		DECLARE_CLASS(InputComponent, Component)
	public:

	public:
		void ClearBindings();
		void RebuildKeyMap(class InputManager* inputManager);



		template<typename ObjectSubclass>
		InputActionBinding& BindAction(const StringID name, const EInputEvent::Type keyEvent, ObjectSubclass* object, typename BoundObjectFunction<void, ObjectSubclass>::MethodPtr f)
		{
			InputActionBinding ab(name, keyEvent);
			ab.IADelegate.BindObject(object, f);
			m_ActionBindings.emplace_back(MakeShared<InputActionBinding>(std::move(ab)));
			InputActionBinding& binding = *m_ActionBindings.back();
			binding.GenNewHandle();
			return *m_ActionBindings.back();
		}

		template<typename ObjectSubclass>
		InputAxisBinding& BindAxis(const StringID name, ObjectSubclass* object, typename BoundObjectFunction<void, ObjectSubclass, float>::MethodPtr f)
		{
			InputAxisBinding ab(name);
			ab.IADelegate.BindObject(object, f);
			m_AxisBindings.emplace_back(std::move(ab));
			return m_AxisBindings.back();
		}
		
		template<typename ObjectSubclass>
		InputAxisKeyBinding& BindAxisKey(const Key key, ObjectSubclass* object, typename BoundObjectFunction<void, ObjectSubclass, float>::MethodPtr f)
		{
			InputAxisKeyBinding ab(key);
			ab.IADelegate.BindObject(object, f);
			m_AxisKeyBindings.emplace_back(std::move(ab));
			return m_AxisKeyBindings.back();
		}

		template<typename ObjectSubclass>
		InputKeyBinding& BindKey(const KeyWithMods keyWithMods, const EInputEvent::Type keyEvent, ObjectSubclass* object, typename BoundObjectFunction<void, ObjectSubclass>::MethodPtr f)
		{
			InputKeyBinding kb(keyWithMods, keyEvent);
			kb.Delegate.BindObject(object, f);
			m_KeyBindings.emplace_back(std::move(kb));
			return m_KeyBindings.back();
		}

		void OnOwnerEndPlay(Entity* entity);
	private:
		void GetActionsBoundToKey(InputManager* inputManager, Key key, std::vector<SharedPtr<InputActionBinding>>& actions) const;

	public:
		int32 Priority;
		uint8 bBlockInput : 1;

	private:
		std::vector<InputKeyBinding> m_KeyBindings;
		std::vector<InputAxisBinding> m_AxisBindings;
		std::vector<InputAxisKeyBinding> m_AxisKeyBindings;
		std::vector<SharedPtr<InputActionBinding>> m_ActionBindings;
		std::vector<KeyToActionInfo> m_KeyToAction;

		friend InputManager;
	};
}