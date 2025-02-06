#pragma once
#include "CoreDefines.h"
#include "Test/FrameBuffer.h"

namespace VeiM
{

	struct CORE_API Vertex
	{
		float Pos[2];
		float TexCoord[2];
	};

	class CORE_API IMesh
	{
	public:
		IMesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices);
		virtual ~IMesh();
		void SetShader(uint32 shader) { m_Shader = shader; }
		uint32 GetShader() { return m_Shader; }
		uint32 GetVAO() { return m_VAO; }
		uint32 GetVBO() { return m_VBO; }
		uint32 GetEBO() { return m_EBO; }

		void Render();

	protected:
		uint32 m_VAO, m_VBO, m_EBO;
		uint32 m_Shader;
		uint32 m_IndexCount;
	};


	class CORE_API QuadMesh : public IMesh
	{
	public:
		QuadMesh() : IMesh(s_Vertices, s_Indices) {}


	private:
		static inline 		std::vector<VeiM::Vertex> s_Vertices = {
		{{-0.5f, -0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f}, {0.0f, 1.0f}}
		};
		static inline 		std::vector<VeiM::uint32> s_Indices = { 0, 1, 2, 2, 0, 3 };
	};

	class  TestRenderer
	{
	public:
		CORE_API static void Init();

		CORE_API static void RenderMesh(IMesh* mesh);
		CORE_API static void Clear();
		CORE_API static void DisplayFramebufferToScreen(FrameBuffer& frameBufferA);

	private:
		static inline float fullscreenQuadVertices[] = {
			// Positions         // Texture coordinates
			-1.0f,  -1.0f, 0.0f,  0.0f,
			 1.0f,  -1.0f, 1.0f,  0.0f,
			 1.0f,   1.0f, 1.0f,  1.0f,
			-1.0f,   1.0f, 0.0f,  1.0f
		};
		static inline unsigned int quadIndices[] = { 0, 1, 2, 2, 3, 0 };
		static inline unsigned int fullscreenQuadVAO, fullscreenQuadVBO, fullscreenQuadEBO;
		static inline const char* vertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec2 aPos; // Position
layout(location = 1) in vec2 aTexCoord; // Texture coordinate

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord; // Pass the texture coordinate to fragment shader
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";
		static inline const char* fragmentShaderSource = R"(
#version 460 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture; // The texture from the framebuffer

void main() {
    FragColor = texture(screenTexture, TexCoord); // Sample the texture
}
)";
		static inline uint32 displayShader;
	};
}