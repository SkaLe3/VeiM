#include "GameClasses.h"

	 using namespace VeiM;

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
    FragColor = vec4(0.7, 0.0, 1.0, 1.0); // Red color
}
)";
	return triangleFragmentShaderSource;
}

float* TriangleMesh::GetData()
{
	return mesh;
}

uint32 TriangleMesh::GetSize()
{
	return 9 * sizeof(float);
}

