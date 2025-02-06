#include "Windows/WindowsUtils.h"

#include <Windows.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <sstream>


namespace VeiM
{
	float Time::GetTime()
	{
		return glfwGetTime();
	}

	CORE_API void GetFramebufferSize(GLFWwindow* window, int32& width, int32& height)
	{
		return glfwGetFramebufferSize(window, &width, &height);
	}


	

}

