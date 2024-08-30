#include "WindowsUtils.h"
#include <GLFW/glfw3.h>

namespace VeiM
{
	float Time::GetTime()
	{
		return glfwGetTime();
	}
}

