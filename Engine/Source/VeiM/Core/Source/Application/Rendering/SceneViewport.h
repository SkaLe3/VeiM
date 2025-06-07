#pragma once
#include "CoreDefines.h"
#include "Application/Rendering/Common.h"
#include "Application/Rendering/Viewport.h"


namespace VeiM
{
	class GameViewportClient;

	class SceneViewport : public Viewport, public IApplicationViewport
	{
	public:
		SceneViewport(GameViewportClient* client);
		void HandleViewportSwap(const SceneViewport& newViewport);

		virtual void UpdateViewportRI();
		/* IApplicationViewport*/
		virtual glm::ivec2 GetSize() const override { return GetSizeXY(); }
	private:
		glm::vec2 m_MouseDelta;


	public:
		virtual uint32 GetRenderTargetTexture() const { return 0; }
	};
}