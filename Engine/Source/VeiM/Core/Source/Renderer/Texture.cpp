#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace VeiM
{


	Texture TextureFromFile(const fs::path& filename)
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
			GLenum format = GL_RGB;

			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			VM_CORE_ERROR("Failed to load texture '{0}'", filename.string());
		}
		stbi_image_free(data);
		return texture;
	}

}

