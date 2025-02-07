#include "GameClasses.h"

using namespace VeiM;



__declspec(dllexport) void gameLog()
{
	VM_WARN("gameLog called");

}



std::string TriangleShader::GetVertex()
{
	const char* triangleVertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

	return triangleVertexShaderSource;
}

std::string TriangleShader::GetFragment()
{
	const char* triangleFragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

void main() {
    FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Red color
}
)";
	return triangleFragmentShaderSource;
}

TriangleMesh::TriangleMesh() : VeiM::IMesh(s_Vertices, s_Indices)
{
	gameLog();
}

std::vector<VeiM::Vertex> TriangleMesh::s_Vertices = {
	{{-0.5f, -0.5f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ 0.0f,  0.5f}, {0.5f, 1.0f}}
};

std::vector<VeiM::uint32> TriangleMesh::s_Indices = { 0, 1, 2 };


namespace
{
	ClassInfo gameCharacterInfo = {
		"GameCharacter",
		{"Name"},
		{"SetName", "GetName", "Attack", "Start", "Update"}
	};
	Base::Registrar<GameCharacter> registrar("GameCharacter", gameCharacterInfo);
}