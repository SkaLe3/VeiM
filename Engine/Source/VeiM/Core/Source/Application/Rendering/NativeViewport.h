#pragma once
#include "CoreDefines.h"
#include "Application/Editor/EditorViewportClient.h"

namespace VeiM
{
	class GameViewportClient;
	class SceneViewport;

	class NativeViewport
	{
	public:
		void StartEditorPlaySession(GameViewportClient* client);

	private:
		SharedPtr<EditorViewportClient> m_EditorViewportClient;
		SharedPtr<SceneViewport> m_ActiveViewport;
		SharedPtr<SceneViewport> m_InactiveViewport;
	};
}