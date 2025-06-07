#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"

namespace VeiM
{

	struct KeyInfo;

	struct CORE_API Key
	{
	public:
		Key() {}
		Key(const StringID name) : m_Name(name) {}
		Key(const char* name) : m_Name(StringID(name)) {}

		bool IsValid();
		bool IsAnalog() const;
		bool IsGamepad() const;
		bool IsAxis() const;

		bool AlwaysUpdateAxis() const;

		friend bool operator==(const Key& a, const Key& b) { return  a.m_Name == b.m_Name; }
		friend bool operator!=(const Key& a, const Key& b) { return a.m_Name != b.m_Name; }
		friend bool operator<(const Key& a, const Key& b) { StringIDComparator sidc; return sidc(a.m_Name, b.m_Name); }

	private:
		void UpdateInfo() const;
	private:
		StringID m_Name;
		mutable SharedPtr<KeyInfo> m_Info;

		friend struct IKey;
		friend struct std::hash<VeiM::Key>;
	};

	struct CORE_API KeyInfo
	{
	public:
		KeyInfo(const Key key, bool bMouse = false, bool bGamePad = false, bool bModifier = false, uint8 axisDimestion = 0, bool bUpdateAxis = false);

		const Key& GetKey() const { return m_Key; }

		bool IsAnalog() const;
		bool IsGamepad() const;
		bool IsAxis() const;
		bool AlwaysUpdateAxis() const;

	private:
		Key m_Key;
		uint8 m_bIsModifier : 1;
		uint8 m_bIsGamepad : 1;
		uint8 m_bIsMouseButton : 1;
		uint8 m_AxisDimension : 2;
		uint8 m_bAlwaysUpdateAxis : 1;

		String m_Name;
		friend struct IKey;
	};

	// I stands for Input
	struct CORE_API IKey
	{
#define REGISTER_KEYCODE(Keycode, Keyname) static const Key Keyname;
#define REGISTER_MOUSE_AXES 1
#include "Windows/ButtonCodesDefines.inl"
#include "Windows/KeyCodesDefines.inl"
		// TODO: Add Gamepad
#undef REGISTER_MOUSE_AXES
#undef REGISTER_KEYCODE
		static const Key Invalid;

	public:
		static void Initialize();
		static void AddKey(const KeyInfo& info);
		static SharedPtr<KeyInfo> GetInfo(const Key key);

	private:
		static std::map<Key, SharedPtr<KeyInfo>> s_KeyMap;
		static bool s_bInitialized;
	};




	class CORE_API KeyInputManager
	{
	public:
		static KeyInputManager& Get();

		void InitKeyMappings();
		Key GetKeyFromKeyCode(const int32 keyCode) const;
	private:
		KeyInputManager();
	private:
		static SharedPtr<KeyInputManager> s_Instance;

		std::unordered_map<int32, Key> m_KeyCodeMap;
	};
}

namespace std
{
	template<>
	struct hash<VeiM::Key> {
		size_t operator()(const VeiM::Key& key) const noexcept {
			return static_cast<size_t>(key.m_Name.GetID());
		}
	};
}