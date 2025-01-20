#pragma once
 
#include "VeiM/Core/CoreDefines.h"
#include "VeiM/Core/Layer.h"

#include "VeiM/Test/FrameBuffer.h"

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
		FrameBuffer m_Framebuffer;
		int display_w, display_h;
		uint32 triangleShader;
		uint32 quadShader;
		uint32 triangleVAO, triangleVBO;
		uint32 quadVAO, quadVBO;

	};
}