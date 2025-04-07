#pragma once
#include "CoreDefines.h"
#include "Renderer/Mesh.h"

namespace VeiM
{
	class CORE_API SphereMesh : public IMesh
	{
	public:
		SphereMesh(uint32 xSegments, uint32 ySegments);
	};
}