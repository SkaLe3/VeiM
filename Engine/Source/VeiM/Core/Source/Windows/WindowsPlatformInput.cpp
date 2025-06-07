#include "WindowsPlatformInput.h"

namespace VeiM
{

	int32 WindowsPlatformInput::PopulateKeyArray(int32* keyCodes, String* keyNames, int32 maxValue)
	{
		int32 currentIndex = 0;
#define REGISTER_KEYCODE(Keycode, Keyname) if (currentIndex < maxValue) {keyCodes[currentIndex] = Keycode; keyNames[currentIndex] = #Keyname; ++currentIndex;};
		if (keyCodes && keyNames && maxValue > 0)
		{
			#include "Windows/KeyCodesDefines.inl"
			#include "Windows/ButtonCodesDefines.inl"
		}
#undef REGISTER_KEYCODE
		return currentIndex;
	}


}