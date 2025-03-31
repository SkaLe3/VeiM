#pragma once
#include "CoreDefines.h"
#include "Test/FrameBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace VeiM
{

	struct CORE_API Vertex
	{
		float Pos[3];
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
		{{-0.5f, -0.5f, 0.f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.f}, {0.0f, 1.0f}}
		};
		static inline 		std::vector<VeiM::uint32> s_Indices = { 0, 1, 2, 2, 0, 3 };
	};

	class CORE_API CubeMesh : public IMesh
	{
	public:
		CubeMesh() : IMesh(s_Vertices, s_Indices) {}


	private:
		static inline 		std::vector<VeiM::Vertex> s_Vertices = {
		{{ 1.f,  1.f, -1.f},{0.625f, 0.500f}},
		{{-1.f,  1.f, -1.f},{0.875f, 0.500f}},
		{{-1.f,  1.f,  1.f},{0.875f, 0.750f}},
		{{ 1.f,  1.f,  1.f},{0.625f, 0.750f}},
		{{ 1.f, -1.f,  1.f},{0.375f, 0.750f}},
		{{ 1.f,  1.f,  1.f},{0.625f, 0.750f}},
		{{-1.f,  1.f,  1.f},{0.625f, 1.000f}},
		{{-1.f, -1.f,  1.f},{0.375f, 1.000f}},
		{{-1.f, -1.f,  1.f},{0.375f, 0.000f}},
		{{-1.f,  1.f,  1.f},{0.625f, 0.000f}},
		{{-1.f,  1.f, -1.f},{0.625f, 0.250f}},
		{{-1.f, -1.f, -1.f},{0.375f, 0.250f}},
		{{-1.f, -1.f, -1.f},{0.125f, 0.500f}},
		{{ 1.f, -1.f, -1.f},{0.375f, 0.500f}},
		{{ 1.f, -1.f,  1.f},{0.375f, 0.750f}},
		{{-1.f, -1.f,  1.f},{0.125f, 0.750f}},
		{{ 1.f, -1.f, -1.f},{0.375f, 0.500f}},
		{{ 1.f,  1.f, -1.f},{0.625f, 0.500f}},
		{{ 1.f,  1.f,  1.f},{0.625f, 0.750f}},
		{{ 1.f, -1.f,  1.f},{0.375f, 0.750f}},
		{{-1.f, -1.f, -1.f},{0.375f, 0.250f}},
		{{-1.f,  1.f, -1.f},{0.625f, 0.250f}},
		{{ 1.f,  1.f, -1.f},{0.625f, 0.500f}},
		{{ 1.f, -1.f, -1.f},{0.375f, 0.500f}},

		};
		static inline 		std::vector<VeiM::uint32> s_Indices = {
		0, 1, 2,  0, 2, 3,
		4, 5, 6,  4, 6, 7,
		8, 9, 10,  8, 10, 11,
		12, 13, 14,  12, 14, 15,
		16, 17, 18,  16, 18, 19,
		20, 21, 22,  20, 22, 23
		};
	};

	class  TestRenderer
	{
	public:
		CORE_API static void Init();

		CORE_API static void RenderMesh(IMesh* mesh, Shader& shader, Texture& tex, const glm::mat4& viewProj, const glm::mat4& view, const glm::mat4& transform);
		CORE_API static void Clear();
		CORE_API static void BlitFramebufferToSwapchain(FrameBuffer& framebuffer);

	};
}