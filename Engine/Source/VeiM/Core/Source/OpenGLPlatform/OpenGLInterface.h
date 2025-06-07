#pragma once
#include "CoreDefines.h"
#include "RenderInterface/RenderInterface.h"

#include <GLFW/glfw3.h>
#include <Glad/glad.h>

namespace VeiM
{
	class OpenGLPlatform
	{
	public:
		static RenderInterface* CreateRI();
	};




	class OpenGLViewport;

	class OpenGLRenderInterface : public RenderInterface
	{
		static inline OpenGLRenderInterface* s_Instance = nullptr;
	public:
		static OpenGLRenderInterface* Get() { return s_Instance; }
		OpenGLRenderInterface();


		// GetVersions
		virtual void Init() override;
		virtual void Shutdown() override {}
	private:
		void InitOpenGL();

	public:
		virtual SharedPtr<ViewportRI> CreateViewport(uint32 width, uint32 height, bool bFullscreen, EPixelFormat format) final override;


	private:
		OpenGLViewport* m_Viewport;

		friend class OpenGLViewport;
	};

	struct OpenGLContext
	{
		GLFWwindow* WindowHandle;
		int32 SyncInterval;
		GLuint ViewportFramebuffer;
		GLuint BackbufferTarget;
		GLenum BackbufferRendererID;
	};


	void BlitToViewport(const OpenGLViewport& viewport, uint32 backbufferWidth, uint32 backbufferHeight, bool bPresent);

	OpenGLContext* CreateOpenGLContext(OpenGLViewport* owningViewport);

	void ResizeOpenGLContext(const OpenGLViewport& viewport, OpenGLContext* context, uint32 width, uint32 height,bool bFullscreen, bool bWasFullscreen, GLenum bbTarget, GLuint bbRendererID);


}