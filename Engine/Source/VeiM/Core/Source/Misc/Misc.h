#pragma once


namespace VeiM
{
	struct PlatformProperties
	{
		static FORCEINLINE bool IsGameOnly()
		{
			return VM_GAME;
		}

		static FORCEINLINE bool IsUnified()
		{
			return IS_UNIFIED;
		}
	};

	FORCEINLINE bool IsRunningGame()
	{
		return PlatformProperties::IsGameOnly();
	}

}