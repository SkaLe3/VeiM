#pragma once
#include "CoreDefines.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>

namespace VeiM
{
	class Shader
	{
	public:
		Shader(const fs::path& filePath);
		Shader(const String& name, const String& vertexSource, const String& fragmentSource);
		~Shader();

		void Bind() const;
		void UnBind() const;

		uint32 GetUniformLocation(const String& uniformName);

		// TODO: Check link in bookmarks
		void SetBool(const String& name, bool value);
		void SetUniformInt(const String& name, int32 value); 
		void SetUnformIntArray(const String& name, int32* values, uint32 count);
		void SetFloat(const String& name, float value);
		void SetFloat2(const String& name, const glm::vec2& value);
		void SetFloat3(const String& name, const glm::vec3& value);
		void SetFloat4(const String& name, const glm::vec4& value);
		void SetMat4(const String& name, const glm::mat4& value);

	private:
		String ReadShaderFile(const fs::path& filePath);
		std::unordered_map<GLenum, String> PreProcess(const String& source);
		void CreateProgram(std::unordered_map<GLenum, String>& sources);

	private:
		uint32 m_RendererID;
		String m_Name;
		fs::path m_ShaderPath;
		std::unordered_map<String, uint32> m_UniformLocationMap;
	};
}