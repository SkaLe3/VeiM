#include "Image.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include <Glad/glad.h>

#include "Application.h"
#include "stb_image.h"


//temp
#include <iostream>

namespace VeiM
{


	Image::Image(uint32_t width, uint32_t height, ImageFormat format)
		: m_Width(width), m_Height(height), m_Format(format)
	{
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Image::Image(const std::string& path)
		: m_Path(path)
	{
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		uint8_t* data = nullptr;

		if (stbi_is_hdr(m_Path.c_str()))
		{
			data = (uint8_t*)stbi_loadf(m_Path.c_str(), &width, &height, &channels, 4);
			m_Format = ImageFormat::RGBA32F;
		}
		else
		{
			data = stbi_load(m_Path.c_str(), &width, &height, &channels, 4);
			m_Format = ImageFormat::RGBA;
		}
		// Make assert
		if (!data)
			std::cout << "Failed to load image" << std::endl;
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		glBindTextureUnit(0, m_ID);




	}

	Image::~Image()
	{
		glDeleteTextures(1, &m_ID);
	}

	void Image::SetData(void* data)
	{
		uint32_t bpp;
		switch (m_Format)
		{
		case ImageFormat::RGB:
			bpp = 3;
			break;
		case ImageFormat::RGBA:
			bpp = 4;
			break;
		case ImageFormat::RGBA32F:
			bpp = 16;
			break;
		}

		//make assert
		size_t size = m_Width * m_Height * bpp;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	void* Image::Decode(const void* buffer, uint64_t length, uint32_t& outWidth, uint32_t& outHeight)
	{
		int width, height, channels;
		uint8_t* data = nullptr;
		uint64_t size = 0;

		data = stbi_load_from_memory((const stbi_uc*)buffer, length, &width, &height, &channels, 4);
		size = width * height * 4;

		outWidth = width;
		outHeight = height;

		return data;
	}

}