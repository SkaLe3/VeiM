#pragma once
#include "CoreDefines.h"
#include "Test/TestRenderer.h"

#include <string>
#include <vector>

extern "C"
{
	__declspec(dllexport) void gameLog();
}

class TriangleShader
{
public:
	std::string	GetVertex();
	std::string GetFragment();
};


class TriangleMesh : public VeiM::IMesh
{
public:
	TriangleMesh();


private:
	static	std::vector<VeiM::Vertex> s_Vertices;
	static	std::vector<VeiM::uint32> s_Indices;
};
