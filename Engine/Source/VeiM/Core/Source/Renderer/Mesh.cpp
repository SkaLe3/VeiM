#include "Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VeiM
{


	IMesh::IMesh() : m_VAO(0), m_VBO(0), m_EBO(0)
	{
		Tdiffuse.Id = 0;
		Tspecualr.Id = 0;
	}

	IMesh::IMesh(const std::vector<glm::vec3>& positions, const std::vector<uint32>& indices) : IMesh()
	{
		Positions = positions;
		Indices = indices;
		Tdiffuse.Id = 0;
		Tspecualr.Id = 0;
	}

	IMesh::IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<uint32>& indices) : IMesh()
	{
		Positions = positions;
		UV = uv;
		Indices = indices;
		Tdiffuse.Id = 0;
		Tspecualr.Id = 0;
	}

	IMesh::IMesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uv, const std::vector<glm::vec3>& normals, const std::vector<uint32>& indices) : IMesh()
	{
		Positions = positions;
		UV = uv;
		Normals = normals;
		Indices = indices;
		Tdiffuse.Id = 0;
		Tspecualr.Id = 0;
	}

	IMesh::~IMesh()
	{
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		glDeleteVertexArrays(1, &m_VAO);
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

	SkyBoxCube::SkyBoxCube()
	{
		SkyboxVertices = {
			// positions          
			{-1.0f,  1.0f, -1.0f},
			{-1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
								
			{-1.0f, -1.0f,  1.0f},
			{-1.0f, -1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f},
								
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
							
			{-1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f},
								
			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f},
							
			{-1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f},
		};
	}

	void SkyBoxCube::Finilize()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, SkyboxVertices.size() * sizeof(glm::vec3), SkyboxVertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(0);
	}

}

