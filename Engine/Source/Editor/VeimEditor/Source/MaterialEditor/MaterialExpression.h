#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class MaterialExpression
	{
	public:
		virtual int32 Compile(class MaterialCompiler* compiler, int32 outputIndex) = 0;


	private:
	};

	struct ExpressionInput
	{
		MaterialExpression* Expression;
		int32 OutputIndex;
	};


	class MaterialExpressionAdd : MaterialExpression
	{
	public:
		virtual int32 Compile(class MaterialCompiler* compiler, int32 outputIndex) override;

	public:
		ExpressionInput m_InputA;
		ExpressionInput m_InputB;
	};
}
