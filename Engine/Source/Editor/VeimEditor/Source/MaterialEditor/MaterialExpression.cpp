#include "MaterialExpression.h"
#include "MaterialEditor/MaterialCompiler.h"

namespace VeiM
{


	int32 MaterialExpressionAdd::Compile(class MaterialCompiler* compiler, int32 outputIndex)
	{
		int32 A = compiler->Compile(m_InputA);
		int32 B = compiler->Compile(m_InputB);
		return compiler->Add(A, B);
	}
}