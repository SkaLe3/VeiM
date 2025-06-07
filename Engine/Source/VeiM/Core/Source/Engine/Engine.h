#pragma once
#include "CoreDefines.h"
#include "Engine/CoreObject.h"
#include "Engine/ObjectPtr.h"
#include "Engine/Reflection.h"

namespace VeiM
{
	class GameViewportClient;

	class CORE_API Engine : public Object
	{
		DECLARE_CLASS(Engine, Object)
	public:
		Engine();

		virtual void Init();
		virtual void Start();
		virtual void Tick(float deltaTime) {}

		void NotifyGCRunThisFrame();
		void ResetRunGCFlag();
		bool HasRunGCThisFrame();
		void DebugDisableEditorCamera();
		void DebugEditorMouseCallback(double x, double y);

	public:
		ObjectPtr<GameViewportClient> GameViewport;

	private:
		// TODO: Add timer manager
		bool m_bGCRunThisFrame;
	};

	extern CORE_API Engine* g_Engine;
}