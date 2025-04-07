#pragma once
#include "CoreDefines.h"

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

	class CORE_API IMesh
	{
	public:
		IMesh();
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<uint32>& indices);
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<uint32>& indices);
		IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<glm::vec3>& normals, const std::vector<uint32>& indices);



		virtual ~IMesh();
		void SetShader(uint32 shader) { m_Shader = shader; }
		uint32 GetShader() { return m_Shader; }
		uint32 GetVAO() { return m_VAO; }
		uint32 GetVBO() { return m_VBO; }
		uint32 GetEBO() { return m_EBO; }

		void Render();

		void Finilize();

	public:
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec2> UV;
		std::vector<glm::vec3> Normals;
		// TODO: Add more

		ETopology Topology = ETopology::Triangles;
		std::vector<uint32> Indices;

	protected:
		uint32 m_VAO, m_VBO, m_EBO;
		uint32 m_Shader;
		uint32 m_IndexCount;
	};
}