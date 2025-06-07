#pragma once
#include "CoreDefines.h"

#include <glm/glm.hpp>
// TODO: MOVE THIS TO RENDERING FOLDER

namespace VeiM
{
	class ViewportRI;

	class CORE_API RenderTarget
	{
	public:
		virtual glm::ivec2 GetSizeXY() const = 0;
	};



	class CORE_API Viewport : public RenderTarget
	{
	public:
		Viewport();

		void SetViewportEditorPlay(bool bEditor);
		bool IsViewportEditorPlay() const;

		/* RenderTarget Interface */
		virtual glm::ivec2 GetSizeXY() const override { return { m_Width, m_Height }; }

		/* ------- */
	protected:
		virtual void UpdateViewportRI(uint32 newWidth, uint32 newHeight, bool bDestroy);

	private:
		SharedPtr<ViewportRI> m_NativeViewport;

		uint32 m_Width;
		uint32 m_Height;

		uint32 m_bIsEditorPlay : 1;
	};
}