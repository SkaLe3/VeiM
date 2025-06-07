#include "OpenGLInterface.h"
#include "OpenGLObjects.h"

#include <GLFW/glfw3.h>
#include <Glad/glad.h>

namespace VeiM
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		VM_CORE_ERROR("[GLFW] Error({0}) : {1}", error, description);
	}
	static void glDebugOutput(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		void* userParam)
	{
		// ignore non-significant error/warning codes
		if (
			id == 131169 || id == 131185 || id == 131218 || id == 131204 || id || // driver-specific non-significant error codes
			id == 2000 || id == 2001 || id == 2265 // shader compilation error codes; ignore as already managed from shader object
			)
		{
			return;
		}
		String logMessage = "Debug output: (" + std::to_string(id) + "): " + message + "\n";

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             logMessage += "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   logMessage += "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: logMessage += "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     logMessage += "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     logMessage += "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           logMessage += "Source: Other"; break;
		} logMessage += "\n";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               logMessage += "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logMessage += "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  logMessage += "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         logMessage += "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         logMessage += "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              logMessage += "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          logMessage += "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           logMessage += "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               logMessage += "Type: Other"; break;
		} logMessage += "\n";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         logMessage += "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       logMessage += "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          logMessage += "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: logMessage += "Severity: notification"; break;
		} logMessage += "\n";
		logMessage += "\n";

		// only log a message a maximum of 3 times (as it'll keep spamming the message queue with
		// the same error message)
		static unsigned int msgCount = 0;
		if (msgCount++ < 3)
		{
			if (type == GL_DEBUG_TYPE_ERROR)
			{
				VM_CORE_ERROR(logMessage);
			}
			else
			{
				VM_CORE_WARN(logMessage);
			}
		}
	}

	static void CreateStubWindowAndContext(OpenGLContext* context)
	{
		static bool bStubWasInitialized = false;
		if (!bStubWasInitialized)
		{
			bStubWasInitialized = true;

			int succeed = glfwInit();
			VM_CORE_ASSERT(succeed, "Could not initialize GLFW");
			VM_CORE_INFO("[GLFW] GLFW Init");

			glfwSetErrorCallback(GLFWErrorCallback);

#if defined(VM_DEBUG)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
			GLFWwindow* stubWindow = glfwCreateWindow(1, 1, "StubWindow", nullptr, nullptr);
			context->WindowHandle = stubWindow;
		}
	}
	

	RenderInterface* OpenGLPlatform::CreateRI()
	{
		return new OpenGLRenderInterface();
	}

	OpenGLRenderInterface::OpenGLRenderInterface()
	{
		VM_CORE_ASSERT(!s_Instance, "Attempted to create OpenGLRenderInterface when it already exists");
		s_Instance = this;
		InitOpenGL();
	}


	void OpenGLRenderInterface::Init()
	{
		glFlush();
	}

	void OpenGLRenderInterface::InitOpenGL()
	{
		static bool bInitialized = false;
		if (!bInitialized)
		{
			OpenGLContext stubContext;
			CreateStubWindowAndContext(&stubContext);
			glfwMakeContextCurrent(stubContext.WindowHandle);
			int32 gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

			VM_CORE_ASSERT(gladStatus, "Failed to initialize Glad!");
			VM_CORE_INFO("OpenGL Info:");
			VM_CORE_INFO("    Vendor: {0}", (char*)glGetString(GL_VENDOR));
			VM_CORE_INFO("    Renderer: {0}", (char*)glGetString(GL_RENDERER));
			VM_CORE_INFO("    Version: {0}", (char*)glGetString(GL_VERSION));
			VM_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "VeiM requires OpenGL version 4.5 or higher");

			VM_CORE_INFO("Initializing debug output");
			int flags;
			glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
			if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				// we succesfully requested a debug context, now initialize
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
			}
			else
			{
				VM_CORE_WARN("Debug output is not supported");
			}

			glfwMakeContextCurrent(nullptr);
			glfwDestroyWindow(stubContext.WindowHandle);
		}
	}

	SharedPtr<ViewportRI> OpenGLRenderInterface::CreateViewport( uint32 width, uint32 height, bool bFullscreen, EPixelFormat format)
	{
		format = (format == EPixelFormat::None) ? EPixelFormat::RGBA8 : format;
		return MakeShared<OpenGLViewport>(this, width, height, bFullscreen, format);
	}



	void BlitToViewport(const OpenGLViewport& viewport, uint32 backbufferWidth, uint32 backbufferHeight, bool bPresent)
	{
		OpenGLContext* const context = viewport.GetContext();

		bool bChangedContext = false;
		GLFWwindow* window = glfwGetCurrentContext();
		if (window != context->WindowHandle)
		{
			bChangedContext = true;
			glfwMakeContextCurrent(context->WindowHandle);
		}
		if (viewport.Dirty)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, context->ViewportFramebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, context->BackbufferTarget, context->BackbufferRendererID, 0);

			GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (result != GL_FRAMEBUFFER_COMPLETE)
			{
				VM_CORE_ERROR("Framebuffer not complete. Status = {}", result);
			}
			viewport.Dirty = false;
		}

		glDisable(GL_FRAMEBUFFER_SRGB);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, context->ViewportFramebuffer);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, backbufferWidth, backbufferHeight,
			0, 0, backbufferWidth, backbufferHeight,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glEnable(GL_FRAMEBUFFER_SRGB);

		if (bPresent)
		{
			glfwSwapBuffers(context->WindowHandle);
		}

		if (bChangedContext)
		{
			glfwMakeContextCurrent(window);
		}
	}

	void InitDefaultOpenGLContext()
	{
		glDisable(GL_DITHER);
		glDisable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	OpenGLContext* CreateOpenGLContext(OpenGLViewport* owningViewport)
	{
		owningViewport->Dirty = true;
		OpenGLContext* context = new OpenGLContext;
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLVersion.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLVersion.minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(VM_DEBUG)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		GLFWwindow* window = glfwCreateWindow(owningViewport->GetSizeXY().x, owningViewport->GetSizeXY().y, owningViewport->GetName(), nullptr, nullptr);
		if (!window)
		{
			VM_CORE_CRITICAL("Failed to create window: {}", owningViewport->GetName());
			return nullptr;
		}
		context->WindowHandle = window;
		
		GLFWwindow* previosContext = glfwGetCurrentContext();
		glfwMakeContextCurrent(window);
		glGenFramebuffers(1, &context->ViewportFramebuffer);
		
		glfwMakeContextCurrent(previosContext);
		return context;
	}

	void ResizeOpenGLContext(const OpenGLViewport& viewport, OpenGLContext* context, uint32 width, uint32 height, bool bFullscreen, bool bWasFullscreen, GLenum bbTarget, GLuint bbRendererID)
	{
		VM_CORE_ASSERT(context, "Invalid OpenGLContext");
		VM_CORE_ASSERT(context->WindowHandle, "Invalid WindowHandle");

		GLFWwindow* window = (GLFWwindow*)context->WindowHandle;
		if (bFullscreen)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(window, monitor, 0, 0, width, height, mode->refreshRate);
		}
		else if (bWasFullscreen)
		{
			glfwSetWindowMonitor(window, nullptr, 100, 100, width, height, 0);
		}
		else
		{
			glfwSetWindowSize(window, width, height);
		}

		viewport.Dirty = true;
		context->BackbufferRendererID = bbRendererID;
		context->BackbufferTarget = bbTarget;

		GLFWwindow* previousWindow = glfwGetCurrentContext();
		glfwMakeContextCurrent(window);

		glBindFramebuffer(GL_FRAMEBUFFER, context->ViewportFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, bbTarget, bbRendererID, 0);

		GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result != GL_FRAMEBUFFER_COMPLETE)
		{
			VM_CORE_CRITICAL("Framebuffer not complete. Status = {}", result);
		}
		glViewport(0, 0, width, height);
		static GLfloat zeroColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearBufferfv(GL_COLOR, 0, zeroColor);

		if (previousWindow != window) {
			glfwMakeContextCurrent(previousWindow);
		}
	}

}

