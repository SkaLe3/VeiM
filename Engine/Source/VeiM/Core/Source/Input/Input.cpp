#include "Input.h"
#include "HAL/PlatformInput.h"

namespace VeiM
{
#define REGISTER_KEYCODE(Keycode, Keyname) const Key IKey::Keyname(#Keyname);
#define REGISTER_MOUSE_AXES 1
#include "Windows/ButtonCodesDefines.inl"
#include "Windows/KeyCodesDefines.inl"
	// TODO: Add Gamepad
#undef REGISTER_MOUSE_AXES
#undef REGISTER_KEYCODE
	const VeiM::Key IKey::Invalid(EStringID::None);

	std::map<VeiM::Key, VeiM::SharedPtr<VeiM::KeyInfo>> IKey::s_KeyMap;
	bool IKey::s_bInitialized;




	void IKey::Initialize()
	{
		if (s_bInitialized) return;
		s_bInitialized = true;

		AddKey(KeyInfo(IKey::AnyKey));
		AddKey(KeyInfo(IKey::MouseX, true, false, false, 1, true));
		AddKey(KeyInfo(IKey::MouseY, true, false, false, 1, true));
		AddKey(KeyInfo(IKey::MouseScrollUp, true));
		AddKey(KeyInfo(IKey::MouseScrollDown, true));
		AddKey(KeyInfo(IKey::MouseScrollAxis, true, false, false, 1, true));

		AddKey(KeyInfo(IKey::ButtonLeft, true));
		AddKey(KeyInfo(IKey::ButtonRight, true));
		AddKey(KeyInfo(IKey::ButtonMiddle, true));
		AddKey(KeyInfo(IKey::Button3, true));
		AddKey(KeyInfo(IKey::Button4, true));
		AddKey(KeyInfo(IKey::Button5, true));
		AddKey(KeyInfo(IKey::Button6, true));
		AddKey(KeyInfo(IKey::Button7, true));

#define REGISTER_KEYCODE(Keycode, Keyname) AddKey(KeyInfo(IKey::Keyname));

#include "Windows/KeyCodesDefines.inl"

#undef REGISTER_KEYCODE
		IKey::GetInfo(IKey::LeftShift)->m_bIsModifier = true;
		IKey::GetInfo(IKey::RightShift)->m_bIsModifier = true;
		IKey::GetInfo(IKey::LeftControl)->m_bIsModifier = true;
		IKey::GetInfo(IKey::RightControl)->m_bIsModifier = true;
		IKey::GetInfo(IKey::LeftAlt)->m_bIsModifier = true;
		IKey::GetInfo(IKey::RightAlt)->m_bIsModifier = true;
		IKey::GetInfo(IKey::LeftSuper)->m_bIsModifier = true;
		IKey::GetInfo(IKey::RightSuper)->m_bIsModifier = true;


		// Add Gamepad 

	}

	void IKey::AddKey(const KeyInfo& info)
	{
		const Key& key = info.GetKey();
		key.m_Info = MakeShared<KeyInfo>(info);
		s_KeyMap.emplace(key, key.m_Info);
	}



	SharedPtr<KeyInfo> IKey::GetInfo(const Key key)
	{
		auto info = s_KeyMap.find(key);
		if (info == s_KeyMap.end())
		{
			return nullptr;
		}
		return info->second;
	}

	SharedPtr<KeyInputManager> KeyInputManager::s_Instance;

	KeyInputManager& KeyInputManager::Get()
	{
		if (!s_Instance)
		{
			s_Instance = SharedPtr<KeyInputManager>(new KeyInputManager());
		}
		return *s_Instance;
	}

	void KeyInputManager::InitKeyMappings()
	{
		static constexpr uint32 maxMappings = 256;
		int32 keyCodes[maxMappings];
		String keyNames[maxMappings];
		int32 mappingSize = PlatformInput::PopulateKeyArray(keyCodes, keyNames, maxMappings);
		m_KeyCodeMap.clear();
		for (int32 i = 0; i < mappingSize; i++)
		{
			Key newKey(keyNames[i].data());
			if (!newKey.IsValid())
			{
				IKey::AddKey(KeyInfo(newKey));
			}
			m_KeyCodeMap.emplace(keyCodes[i], newKey);
		}
	}

	Key KeyInputManager::GetKeyFromKeyCode(const int32 keyCode) const
	{
		const Key* keyPtr = nullptr;
		auto it = m_KeyCodeMap.find(keyCode);
		if (it != m_KeyCodeMap.end())
		{
			keyPtr = &it->second;
		}
		return keyPtr ? *keyPtr : IKey::Invalid;

	}

	KeyInputManager::KeyInputManager()
	{
		InitKeyMappings();
	}


	KeyInfo::KeyInfo(const Key key, bool bMouse, bool bGamePad, bool bModifier, uint8 axisDimestion, bool bUpdateAxis)
		: m_Key(key)
		, m_bIsMouseButton(bModifier)
		, m_bIsGamepad(bGamePad)
		, m_bIsModifier(bModifier)
		, m_AxisDimension(axisDimestion)
		, m_bAlwaysUpdateAxis(bUpdateAxis)
	{
		m_bIsModifier = 0;
		m_bIsGamepad = 0;
		m_bIsMouseButton = 0;
	}

	bool KeyInfo::IsAnalog() const
	{
		return m_AxisDimension > 0;
	}

	bool KeyInfo::IsGamepad() const
	{
		return m_bIsGamepad;
	}

	bool KeyInfo::IsAxis() const
	{
		return m_AxisDimension != 0;
	}

	bool KeyInfo::AlwaysUpdateAxis() const
	{
		return m_bAlwaysUpdateAxis;
	}

	bool Key::IsValid()
	{
		if (m_Name != EStringID::None)
		{
			UpdateInfo();
			return m_Info != nullptr;
		}
		return false;
	}

	bool Key::IsAnalog() const
	{
		UpdateInfo();
		return m_Info ? m_Info->IsAnalog() : false;
	}

	bool Key::IsGamepad() const
	{
		UpdateInfo();
		return m_Info ? m_Info->IsGamepad() : false;
	}

	bool Key::IsAxis() const
	{
		UpdateInfo();
		return m_Info ? m_Info->IsAxis() : false;
	}

	bool Key::AlwaysUpdateAxis() const
	{
		UpdateInfo();
		return m_Info ? m_Info->AlwaysUpdateAxis() : false;
	}

	void Key::UpdateInfo() const
	{
		if (!m_Info)
		{
			m_Info = IKey::GetInfo(*this);
		}
	}



}

