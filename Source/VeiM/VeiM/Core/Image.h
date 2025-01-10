#pragma once
#include <cstdint>
#include <string>


// TODO: Move to Editor project
namespace VeiM
{

	enum class ImageFormat
	{
		None = 0,
		RGBA,
		RGBA32F,
		RGB
	};

	class Image
	{
	public:
		Image(uint32_t width, uint32_t height, ImageFormat format);
		Image(const std::string& path);
		~Image();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		void SetData(void* data);
		uint32_t GetData() const { return m_ID; }

		static void* Decode(const void* buffer, uint64_t length, uint32_t& outWidth, uint32_t& outHeight);

	private:
		std::string m_Path;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_ID;
		ImageFormat m_Format = ImageFormat::None;
	};
}