#pragma once
#include "CoreDefines.h"

#include "MaterialEditor/MaterialCompiler.h"

namespace VeiM
{

	class VMSLMaterialTranslator : public MaterialCompiler
	{
	private:
		virtual int32 Compile(class ExpressionInput& input) override;


	private:
		virtual int32 Add(int32 A, int32 B) override;
		virtual int32 Multiply(int32 A, int32 B) override;
		virtual int32 TextureSample(int32 textureIndex, int32 coordinateIndex) override; // Consider having mip value

	private:
		const String& GetCode(int32 codeIndex);
		const EMaterialValueType GetType(int32 codeIndex);
		EMaterialValueType GetResultType(int32 A, int32 B);

		template<typename... Args>
		int32 AddCodeChunk(EMaterialValueType type, const String& format, Args&&... args)
		{
			String codeString = std::vformat(format, std::make_format_args(args...));

			int32 tempIndex = m_NextFreeTemp++;

			ShaderCodeChunk chunk;
			chunk.CodeString = codeString;
			chunk.Type = type;
			chunk.TempIndex = tempIndex;

			m_CodeChunks.push_back(chunk);
			int32 chunkIndex = m_CodeChunks.size() - 1;
			return chunkIndex;
		}

	private:
		std::vector<ShaderCodeChunk> m_CodeChunks;
		int32 m_NextFreeTemp = 0;

	};
}