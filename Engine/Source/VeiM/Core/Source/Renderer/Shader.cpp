#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

namespace VeiM
{
	namespace Utils
	{
		static GLenum ShaderTypeFromString(const String& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			VM_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}
	}


	Shader::Shader(const fs::path& filePath) : m_ShaderPath(filePath)
	{
		String source = ReadShaderFile(filePath);
		auto shaderSources = PreProcess(source);
		
		{
			// TODO: Make timer to show how long shader creation took
			CreateProgram(shaderSources);
		}

		m_Name = filePath.stem().string();
	}



	Shader::Shader(const String& name, const String& vertexSource, const String& fragmentSource)
	{
		std::unordered_map<GLenum, String> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;

		CreateProgram(sources);
	}

	Shader::~Shader()
	{

	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::UnBind() const
	{
		glUseProgram(0);
	}

	uint32 Shader::GetUniformLocation(const String& uniformName)
	{
		auto uit = m_UniformLocationMap.find(uniformName);
		if (uit != m_UniformLocationMap.end())
			return uit->second;

		uint32 uniformLocation = glGetUniformLocation(m_RendererID, uniformName.c_str());

		if (uniformLocation == GL_INVALID_INDEX)
		{
			VM_CORE_ERROR("Uniform {0} not found in shader '{1}'", uniformName, m_Name);
			return -1;
		}
		m_UniformLocationMap.emplace(uniformName, uniformLocation);
		return uniformLocation;
	}

	void Shader::SetBool(const String& name, bool value)
	{
		glUniform1i(GetUniformLocation(name), (int)value);
	}

	void Shader::SetUniformInt(const String& name, int32 value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUnformIntArray(const String& name, int32* values, uint32 count)
	{
		glUniform1iv(GetUniformLocation(name), count, values);
	}

	void Shader::SetFloat(const String& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetFloat2(const String& name, const glm::vec2& value)
	{
		glUniform2f(GetUniformLocation(name), value.x, value.y);
	}

	void Shader::SetFloat3(const String& name, const glm::vec3& value)
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void Shader::SetFloat4(const String& name, const glm::vec4& value)
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void Shader::SetMat4(const String& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	String Shader::ReadShaderFile(const fs::path& filePath)
	{
		String result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				VM_CORE_ERROR("Could not read shader source from file '{0}'", filePath.string());
			}
		}
		else
		{
			VM_CORE_ERROR("Could not open shader source file '{0}'", filePath.string());
		}
		return result;
	}

	std::unordered_map<GLenum, String> Shader::PreProcess(const String& source)
	{
		std::unordered_map<GLenum, String> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != String::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			VM_CORE_ASSERT(eol != String::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			String type = source.substr(begin, eol - begin);
			VM_CORE_ASSERT(Utils::ShaderTypeFromString(type) == GL_VERTEX_SHADER || Utils::ShaderTypeFromString(type) == GL_FRAGMENT_SHADER, "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			VM_CORE_ASSERT(nextLinePos != String::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == String::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
		return shaderSources;
	}

	void Shader::CreateProgram(std::unordered_map<GLenum, String>& sources)
	{
		std::unordered_map<GLenum, uint32> shaderIDs;
		char infoLog[512];
		for (const std::pair<GLenum, String>& shaderSource : sources)
		{
			int32 bSuccess;
			const char* shaderSourceString = sources[shaderSource.first].c_str();
			shaderIDs.emplace(shaderSource.first, glCreateShader(shaderSource.first));
			glShaderSource(shaderIDs[shaderSource.first], 1, &shaderSourceString, NULL);
			glCompileShader(shaderIDs[shaderSource.first]);
			glGetShaderiv(shaderIDs[shaderSource.first], GL_COMPILE_STATUS, &bSuccess);
			if (!bSuccess)
			{
				bool bS = bSuccess;
				glGetShaderInfoLog(shaderIDs[shaderSource.first], 512, NULL, infoLog);;
				VM_CORE_ASSERT(bS);
				VM_CORE_ERROR("{1} : ***** shader compilation failed: {0}", infoLog, m_ShaderPath.string()); // TODO: Add enum to string for shader type name
			}
		}

		int32 bSuccess;
		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, shaderIDs[GL_VERTEX_SHADER]);
		glAttachShader(m_RendererID, shaderIDs[GL_FRAGMENT_SHADER]);
		glLinkProgram(m_RendererID);
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &bSuccess);
		if (!bSuccess)
		{
			glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
			VM_CORE_ASSERT(bSuccess);
			VM_CORE_ERROR("Shader linking failed: {0}", infoLog);
		}
		glDeleteShader(shaderIDs[GL_VERTEX_SHADER]);
		glDeleteShader(shaderIDs[GL_FRAGMENT_SHADER]);
	}
}

