#pragma once
#include "CoreDefines.h"

namespace VeiM
{

	enum class EMaterialValueType : uint8
	{
		Unknown = 0,
		Statement,

		Int,

		Float,
		Vec2,
		Vec3,
		Vec4,

		Mat3,
		Mat4,

		Texture2D,
		TextureCube

		// Add Material attributes ?
	};

	struct ShaderCodeChunk
	{
		String CodeString;
		EMaterialValueType Type;
		int32 TempIndex;
	};

	class MaterialCompiler
	{
	public:
		virtual int32 Compile(class ExpressionInput& input) = 0;

	public:
		virtual int32 Add(int32 A, int32 B) = 0;
		virtual int32 Multiply(int32 A, int32 B) = 0;
		virtual int32 TextureSample(int32 textureIndex, int32 coordinateIndex) = 0; // Consider having mip value

	public:
		// Move to utils namespace
		static bool IsScalar(EMaterialValueType type)
		{
			return type == EMaterialValueType::Float;
		}
		static bool IsVector(EMaterialValueType type)
		{
			return type == EMaterialValueType::Vec2 || type == EMaterialValueType::Vec3 || type == EMaterialValueType::Vec4;
		}
		static bool IsMatrix(EMaterialValueType type)
		{
			return type == EMaterialValueType::Mat3 || type == EMaterialValueType::Mat4;
		}
		static int32 GetNumComponents(EMaterialValueType type)
		{
			switch (type)
			{
			case EMaterialValueType::Float: return 1;
			case EMaterialValueType::Vec2: return 2;
			case EMaterialValueType::Vec3: return 3;
			case EMaterialValueType::Vec4: return 4;
			default: return 0;
			}
		}

	};

}
