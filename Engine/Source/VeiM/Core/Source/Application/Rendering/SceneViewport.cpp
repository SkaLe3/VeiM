#include "SceneViewport.h"

namespace VeiM
{

	SceneViewport::SceneViewport(GameViewportClient* client)
		:/* Viewport(client),*/
		m_MouseDelta({0.0f, 0.0f})
	{

	}

	void SceneViewport::HandleViewportSwap(const SceneViewport& newViewport)
	{
		if (GetSizeXY() != newViewport.GetSizeXY())
		{

		}
	}

	void SceneViewport::UpdateViewportRI()
	{

	}

}

