#include "Viewport.h"

namespace VeiM
{
	Viewport::Viewport()
		: m_bIsEditorPlay(false)
	{

	}

	void Viewport::SetViewportEditorPlay(bool bEditor)
	{
		m_bIsEditorPlay = bEditor;
	}

	bool Viewport::IsViewportEditorPlay() const
	{
		return m_bIsEditorPlay;
	}

	void Viewport::UpdateViewportRI(uint32 newWidth, uint32 newHeight, bool bDestroy)
	{
		m_Width = newWidth;
		m_Height = newHeight;

		if (bDestroy)
		{
			m_NativeViewport = nullptr;
		}
		else
		{
			
		}

	}

}

