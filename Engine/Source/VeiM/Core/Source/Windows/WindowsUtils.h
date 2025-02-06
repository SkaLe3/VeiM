#pragma once

#include "CoreDefines.h"

struct GLFWwindow;
namespace VeiM
{
	class Time
	{
	public:
		static float GetTime();
	};


	CORE_API void GetFramebufferSize(GLFWwindow*, int32&, int32&);

}