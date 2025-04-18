#include "VMSLMaterialTranslator.h"
#include "MaterialEditor/MaterialExpression.h"

namespace VeiM
{

	int32 VMSLMaterialTranslator::Compile(ExpressionInput& input)
	{
		return input.Expression->Compile(this, input.OutputIndex);
	}

	int32 VMSLMaterialTranslator::Add(int32 A, int32 B)
	{
		return AddCodeChunk(EMaterialValueType::Float, "({0} + {1})", GetCode(A), GetCode(B));
	}

	int32 VMSLMaterialTranslator::Multiply(int32 A, int32 B)
	{
		return 0;
	}

	int32 VMSLMaterialTranslator::TextureSample(int32 textureIndex, int32 coordinateIndex)
	{
		return 0;
	}

	const String& VMSLMaterialTranslator::GetCode(int32 codeIndex)
	{
		return m_CodeChunks[codeIndex].CodeString;
	}



	const EMaterialValueType VMSLMaterialTranslator::GetType(int32 codeIndex)
	{
		return m_CodeChunks[codeIndex].Type;
	}

	EMaterialValueType VMSLMaterialTranslator::GetResultType(int32 A, int32 B)
	{
		EMaterialValueType typeA = GetType(A);
		EMaterialValueType typeB = GetType(B);

		if (typeA == typeB)
			return typeA;
		
		if (IsScalar(typeA))
		{
			return typeB;
		}
		if (IsScalar(typeB))
		{
			return typeA;
		}
		if (IsVector(typeA) && IsVector(typeB))
		{
			if (GetNumComponents(typeA) == GetNumComponents(typeB))
			{
				return typeA;
			}
			else
			{
				// Error
				return EMaterialValueType::Unknown;
			}
		}

		if (IsMatrix(typeA) || IsMatrix(typeB))
		{
			return EMaterialValueType::Unknown;
				// TODO: research glsl rules 
		}
	}

}

