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
		std::vector<float> internalData;
		internalData.reserve(Positions.size() * 3 + UV.size() * 2 + Normals.size() * 3);
		for (size_t i = 0; i < Positions.size(); i++)
		{
			internalData.push_back(Positions[i].x);
			internalData.push_back(Positions[i].y);
			internalData.push_back(Positions[i].z);
			if (!UV.empty())
			{
				internalData.push_back(UV[i].x);
				internalData.push_back(UV[i].y);
			}
			if (!Normals.empty())
			{
				internalData.push_back(Normals[i].x);
				internalData.push_back(Normals[i].y);
				internalData.push_back(Normals[i].z);
			}
		}
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, internalData.size() * sizeof(float), internalData.data(), GL_STATIC_DRAW);
		if (!Indices.empty())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), Indices.data(), GL_STATIC_DRAW);
		}

		size_t stride = 3 * sizeof(float);
		if (!UV.empty())		stride += 2 * sizeof(float);
		if (!Normals.empty())	stride += 3 * sizeof(float);

		size_t offset = 0;

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += 3 * sizeof(float);

		if (!UV.empty())
		{
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
			glEnableVertexAttribArray(1);
			offset += 2 * sizeof(float);
		}
		if (!Normals.empty())
		{
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
			glEnableVertexAttribArray(2);
			offset += 3 * sizeof(float);

		}
		glBindVertexArray(0);
	}

}

