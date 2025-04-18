#pragma once
#include "CoreDefines.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#include <glm/glm.hpp>

namespace VeiM
{
	enum class ETopology
	{
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	struct VertexAttributeLayout
	{
		int32 Location;
		int32 Components;
		size_t Offset;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 UV;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	class CORE_API IMesh
	{
	public:
		IMesh();
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<uint32>& indices);
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<uint32>& indices);
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<glm::vec3>& normals, const std::vector<uint32>& indices);
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<glm::vec3>& normals, 
			const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents, const std::vector<uint32>& indices);



		virtual ~IMesh();
		void SetShader(uint32 shader) { m_Shader = shader; }
		uint32 GetShader() { return m_Shader; }
		uint32 GetVAO() { return m_VAO; }
		uint32 GetVBO() { return m_VBO; }
		uint32 GetEBO() { return m_EBO; }

		void Finilize();

	public:
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec2> UV;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec3> Tangents;
		std::vector<glm::vec3> Bitangents;
		// TODO: Add more

		ETopology Topology = ETopology::Triangles;
		std::vector<uint32> Indices;

		Texture Tdiffuse;
		Texture Tspecualr;
		Texture Tnormal;

	protected:
		std::vector<VertexAttributeLayout> m_Layout;
		uint32 m_VAO, m_VBO, m_EBO;
		uint32 m_Shader;
		uint32 m_IndexCount;

	};
	
	struct SkyBoxCube
	{
		SkyBoxCube();
		void Finilize();
		std::vector<glm::vec3> SkyboxVertices;
		uint32 VAO;
		uint32 VBO;
	};
}