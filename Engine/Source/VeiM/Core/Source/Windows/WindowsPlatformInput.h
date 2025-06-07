#pragma once
#include "CoreDefines.h"

namespace VeiM
{
#define REGISTER_KEYCODE(Keycode, Keyname) Keyname = Keycode,
	enum class EKeyCode : uint16_t
	{
		#include "Windows/KeyCodesDefines.inl"
	};
	enum class EButtonCode : uint16_t
	{
		#include "Windows/ButtonCodesDefines.inl"
	};

#undef REGISTER_KEYCODE

	struct WindowsPlatformInput
	{
		static int32 PopulateKeyArray(int32* keyCodes, String* keyNames, int32 maxValue);
	};
}