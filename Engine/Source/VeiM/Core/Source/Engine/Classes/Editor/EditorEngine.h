#pragma once
#include "CoreDefines.h"
#include "Engine/Engine.h"

namespace VeiM
{
	class World;
	class NativeViewport;

	class CORE_API EditorEngine : public Engine
	{
		DECLARE_CLASS(EditorEngine, Engine)
	public:
		EditorEngine();

		void RequestPlaySession(std::weak_ptr<NativeViewport> viewport);
		void RequestEndPlaySession();
		void StartPlaySession(std::weak_ptr<NativeViewport> viewport);
		void EndPlaySession();

		virtual void Tick(float deltaTime) override;
		virtual void Init() override;

		World* NewMap();
		void DestroyWorld(World* inWorld);


		void CloseEditor();

		World* CreatePlayWorldDuplication(World* inWorld);

		World* GetEditorWorld() const;
		World* GetPlayWorld() const;
	public:
		ObjectPtr<World> PlayWorld;
		ObjectPtr<World> EditorWorld; 
		bool bIsInitialized;
		bool bRequestedPlaySession;
		bool bRequestedEndPlaySession;
	protected:
		std::weak_ptr<NativeViewport> m_RequestedViewport;
	private:

		// TODO: Here viewports + markreferences of them
	};
}