#pragma once
#include "VeiM/Core/CoreDefines.h"

#include <string>

class TriangleShader
{
public:
	std::string	GetVertex();
	std::string GetFragment();
};


class TriangleMesh
{
public:
	float* GetData();
	VeiM::uint32 GetSize();
private:
	float mesh[9] = {
-0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 0.0f,  0.5f, 0.0f
	};
};