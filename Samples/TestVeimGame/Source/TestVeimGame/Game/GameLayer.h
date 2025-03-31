#pragma once

#ifdef IS_UNIFIED

#include "CoreDefines.h"
#include "Application/Layer.h"
#include "Test/TestRenderer.h"

#include "Test/FrameBuffer.h"

namespace VeiM
{
	class GameLayer : public Layer
	{
	public:
		GameLayer();
		void OnAttach() override;
		void OnUpdate(float deltaTime) override;


	private:
		// Temp
// 		FrameBuffer m_Framebuffer;
// 		int display_w, display_h;
// 		IMesh* m_Mesh;

	};
}

#endif