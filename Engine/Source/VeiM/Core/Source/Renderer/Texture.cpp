#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace VeiM
{


	// TODO: Add sRGB as default
	Texture TextureFromFile(const fs::path& filename, ETextureColorSpace colorSpace)
	{
		Texture texture;
		glGenTextures(1, &texture.Id);
		glBindTexture(GL_TEXTURE_2D, texture.Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Use GL_CLAMP_TO_EDGE for transparent textures

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(filename.string().c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum internalFormat = GL_SRGB8;
			GLenum dataFormat = GL_RGB;

			if (nrChannels == 1)
			{
				internalFormat = GL_R8;
				dataFormat = GL_RED;

			}
			else if (nrChannels == 3)
			{
				internalFormat = (colorSpace == ETextureColorSpace::sRGB) ? GL_SRGB8 : GL_RGB8;
				dataFormat = GL_RGB;

			}
			else if (nrChannels == 4)
			{
				internalFormat = (colorSpace == ETextureColorSpace::sRGB) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			VM_CORE_ERROR("Failed to load texture '{0}'", filename.string());
		}
		stbi_image_free(data);
		return texture;
	}

	VeiM::CubeMap loadCubemap(const std::vector<fs::path>& faces)
	{
		CubeMap cubemap;
		glGenTextures(1, &cubemap.Id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.Id);
		stbi_set_flip_vertically_on_load(false);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].string().c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum internalFormat = GL_SRGB8;
				GLenum dataFormat = GL_RGB;

				if (nrChannels == 1)
				{
					internalFormat = GL_RED;
					dataFormat = GL_RED;

				}
				else if (nrChannels == 3)
				{
					internalFormat = GL_SRGB8;
					dataFormat = GL_RGB;

				}
				else if (nrChannels == 4)
				{
					internalFormat = GL_SRGB8_ALPHA8;
					dataFormat = GL_RGBA;
				}

				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				VM_CORE_ERROR("Failed to load cubemap texture at path: {0}", faces[i].string());
				stbi_image_free(data);
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		return cubemap;
	}

}

