#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class Controller;

	class CORE_API GameStatics
	{
	public:
		static Controller* GetController(int32 index);
	};
}