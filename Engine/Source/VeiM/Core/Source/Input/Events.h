#pragma once
#include "CoreDefines.h"
#include "HAL/PlatformInput.h"
#include "Input/Input.h"

#include <glm/glm.hpp>
#include <unordered_set>

namespace VeiM
{

	struct ModifierState
	{

	public:
		ModifierState()
			: m_bLeftShift(false)
			, m_bRightShift(false)
			, m_bLeftControl(false)
			, m_bRightControl(false)
			, m_bLeftAlt(false)
			, m_bRightAlt(false)
			, m_bLeftSuper(false)
			, m_bRightSuper(false)
			, m_bCapsLock(false)
			, m_bNumLock(false)
		{
		}
		void UpdateFromKeyEvent(int32 keycode, bool bState)
		{
			EKeyCode key = (EKeyCode)keycode;
			switch (key)
			{
			case VeiM::EKeyCode::LeftShift:
				m_bLeftShift = bState;
				break;
			case VeiM::EKeyCode::RightShift:
				m_bRightShift = bState;
				break;
			case VeiM::EKeyCode::LeftControl:
				m_bLeftControl = bState;
				break;
			case VeiM::EKeyCode::RightControl:
				m_bRightControl = bState;
				break;
			case VeiM::EKeyCode::LeftAlt:
				m_bLeftAlt = bState;
				break;
			case VeiM::EKeyCode::RightAlt:
				m_bRightAlt = bState;
				break;
			case VeiM::EKeyCode::LeftSuper:
				m_bLeftSuper = bState;
				break;
			case VeiM::EKeyCode::RightSuper:
				m_bRightSuper = bState;
				break;
			case VeiM::EKeyCode::CapsLock:
				m_bCapsLock = bState;
				break;
			case VeiM::EKeyCode::NumLock:
				m_bNumLock = bState;
				break;
			default:
				break;
			}
		}

		bool IsShiftDown() const { return m_bLeftShift || m_bRightShift; }
		bool IsCtrlDown() const { return m_bLeftControl || m_bRightControl; }
		bool IsAltDown() const { return m_bLeftAlt || m_bRightAlt; }
		bool IsSuperDown() const { return m_bLeftSuper || m_bRightSuper; }
		bool IsCapsLocked() const { return m_bCapsLock; }
		bool IsNumLocked() const { return m_bNumLock; }

	private:
		uint16 m_bLeftShift : 1;
		uint16 m_bRightShift : 1;
		uint16 m_bLeftControl : 1;
		uint16 m_bRightControl : 1;
		uint16 m_bLeftAlt : 1;
		uint16 m_bRightAlt : 1;
		uint16 m_bLeftSuper : 1;
		uint16 m_bRightSuper : 1;
		uint16 m_bCapsLock : 1;
		uint16 m_bNumLock : 1;
	};

	struct InputEvent
	{
	public:
		virtual ~InputEvent() = default;
		InputEvent()
			: m_Mods(ModifierState())
			, m_bRepeat(false)
		{}
		InputEvent(const ModifierState& mods, const bool bRepeat)
			: m_Mods(mods), m_bRepeat(bRepeat)
		{}
		bool IsRepeat() const { return m_bRepeat; }
		bool IsShiftDown() const { return m_Mods.IsShiftDown(); }
		bool IsCtrlDown() const { return m_Mods.IsCtrlDown(); }
		bool IsAltDown() const { return m_Mods.IsAltDown(); }
		bool IsSuperDown() const { return m_Mods.IsSuperDown(); }
		bool IsCapsLocked() const { return m_Mods.IsCapsLocked(); }
		bool IsNumLocked() const { return m_Mods.IsNumLocked(); }
		virtual bool IsKeyEvent() const { return false; }
		virtual bool IsMouseEvent() const { return false; }


	private:
		ModifierState m_Mods;
		bool m_bRepeat;
	};


	struct InputKeyEvent : public InputEvent
	{
	public:
		InputKeyEvent()
			: InputEvent()
			, m_Key()
			, m_KeyCode(0)
			, m_ScanCode(0)
		{ }
		InputKeyEvent(
			const ModifierState& mods,
			const Key key,
			const int32 keycode,
			const int32 scancode,
			bool bRepeat)
			: InputEvent(mods, bRepeat)
			, m_Key(key)
			, m_KeyCode(keycode)
			, m_ScanCode(scancode)
		{ }

		Key GetKey() const { return m_Key; }
		int32 GetCode() const { return m_KeyCode; }
		int32 GetScan() const { return m_ScanCode; }
		virtual bool IsKeyEvent() const override { return true; }
 
	private:
		Key m_Key;
		int32 m_KeyCode;
		int32 m_ScanCode;
	};

	struct AnalogInputEvent : public InputKeyEvent
	{
	public:
		AnalogInputEvent()
			: InputKeyEvent()
			, m_Value(0.f)
		{
		}
		AnalogInputEvent(
			const ModifierState& mods,
			const Key key,
			const int32 keycode,
			const int32 scancode,
			bool bRepeat,
			const float value)
			: InputKeyEvent(mods, key, keycode, scancode, bRepeat)
			, m_Value(value)
		{
		}
		float GetValue() const { return m_Value; }
	private:
		float m_Value;
	};

	struct InputKeyTypeEvent : public InputEvent
	{
	public:
		InputKeyTypeEvent()
			: InputEvent()
			, m_Char(0)
		{}
		InputKeyTypeEvent(
		const ModifierState& mods,
		const char c,
		const bool bRepeat)
			: InputEvent(mods, bRepeat)
			, m_Char(c)
		{ }

		char GetChar() const { return m_Char; }

	private:
		char m_Char;
	};

	struct InputMouseEvent : public InputEvent
	{
	public:
		InputMouseEvent()
			: m_PointerPosition({0.f, 0.f})
			, m_LastPointerPosition({0.f, 0.f})
			, m_PointerOffset({0.f, 0.f})
			, m_PressedButtons()
			, m_CurrentButton()
			, m_WheelOffset(0.f)
		{}

		InputMouseEvent(
		const ModifierState& mods,
		const glm::vec2& pos,
		const glm::vec2& lastpos,
		const std::unordered_set<Key>& buttons,
		const Key button,
		const float wheelOffset)
			: InputEvent(mods, false)
			, m_PointerPosition(pos)
			, m_LastPointerPosition(lastpos)
			, m_PointerOffset(pos - lastpos)
			, m_PressedButtons(buttons)
			, m_CurrentButton(button)
			, m_WheelOffset(wheelOffset)
		{}

		const glm::vec2& GetPos() const { return m_PointerPosition; }
		const glm::vec2& GetLastPos() const { return m_LastPointerPosition; }
		const glm::vec2& GetOffset() const { return m_PointerOffset; }
		Key GetButton() const { return m_CurrentButton; }
		bool IsButtonDown(Key button) const { return m_PressedButtons.count(button) > 0; }
		float GetWheelOffset() const { return m_WheelOffset; }
		const std::unordered_set<Key>& GetPressedButtons() const { return m_PressedButtons; }

		virtual bool IsMouseEvent() const override { return true; }

	private:
		glm::vec2 m_PointerPosition;
		glm::vec2 m_LastPointerPosition;
		glm::vec2 m_PointerOffset;
		const std::unordered_set<Key> m_PressedButtons;
		Key m_CurrentButton;
		float m_WheelOffset;
	};
}