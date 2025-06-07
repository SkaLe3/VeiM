#include "NativeViewport.h"
#include "Application/Rendering/SceneViewport.h"

namespace VeiM 
{

	void NativeViewport::StartEditorPlaySession(GameViewportClient* client)
	{
#if 0
		m_EditorViewportClient->SaveCameraInfo();
		m_InactiveViewport = m_ActiveViewport;

		m_ActiveViewport = MakeShared<SceneViewport>(client);
		m_ActiveViewport->SetViewportEditorPlay(true);
#endif

	}


}

