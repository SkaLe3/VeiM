#include "Engine.h"

#include "Engine/GameViewportClient.h"
#include "Application/Application.h" // Temp
namespace VeiM
{
	IMPLEMENT_CLASS(Engine);


	void Engine::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(Engine, ObjectProperty, GameViewport);
	}

	Engine::Engine()
		:m_bGCRunThisFrame(false)
	{

	}

	void Engine::Init()
	{
		MarkAsRoot();
		// LoadConfig();

	}

	void Engine::Start()
	{

	}

	bool Engine::HasRunGCThisFrame()
	{
		return m_bGCRunThisFrame;
	}

	void Engine::DebugDisableEditorCamera()
	{
		s_EditorCameraEnabled = false;
	}

	void Engine::DebugEditorMouseCallback(double x, double y)
	{
		mouse_callback(x, y);
	}

	void Engine::NotifyGCRunThisFrame()
	{
		m_bGCRunThisFrame = true;
	}

	void Engine::ResetRunGCFlag()
	{
		m_bGCRunThisFrame = false;
	}

}