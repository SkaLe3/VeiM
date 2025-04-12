#include "Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VeiM
{


	IMesh::IMesh() : m_VAO(0), m_VBO(0), m_EBO(0)
	{

	}

	IMesh::IMesh(const std::vector<glm::vec3>& positions, const std::vector<uint32>& indices) : IMesh()
	{
		Positions = positions;
		Indices = indices;
	}

	IMesh::IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<uint32>& indices) : IMesh()
	{
		Positions = positions;
		UV = uv;
		Indices = indices;
	}

	IMesh::IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<glm::vec3>& normals, const std::vector<uint32>& indices) : IMesh()
	{
		Positions = positions;
		UV = uv;
		Normals = normals;
		Indices = indices;
	}

	IMesh::~IMesh()
	{
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		glDeleteVertexArrays(1, &m_VAO);
	}

	void IMesh::Render()
	{
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	void IMesh::Finilize()
	{
		if (!m_VAO)
		{
			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(1, &m_VBO);
			glGenBuffers(1, &m_EBO);
		}

		size_t vertexCount = Positions.size();
		std::vector<Vertex> vertexData(vertexCount);

		for (size_t i = 0; i < vertexCount; i++)
		{
			vertexData[i].Position = Positions[i];
			vertexData[i].UV = (!UV.empty() ? UV[i] : glm::vec2(0.0f));
			vertexData[i].Normal = (!Normals.empty() ? Normals[i] : glm::vec3(0.0f));
		}

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);
		if (!Indices.empty())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), Indices.data(), GL_STATIC_DRAW);
		}

		size_t stride = sizeof(Vertex);

		m_Layout.clear();
		m_Layout.push_back({ 0, 3, offsetof(Vertex, Position) });
		m_Layout.push_back({ 1, 2, offsetof(Vertex, UV) });
		m_Layout.push_back({ 2, 3, offsetof(Vertex, Normal) });

		for (const auto& attr : m_Layout)
		{
			glEnableVertexAttribArray(attr.Location);
			glVertexAttribPointer(attr.Location, attr.Components, GL_FLOAT, GL_FALSE, stride, (void*)(attr.Offset));
		}
		glBindVertexArray(0);
	}

}

