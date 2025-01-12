#include "DesktopPlatformModule.h"
#include "DesktopPlatformWindows.h"

DesktopPlatformModule DesktopPlatformModule::m_Instance;

DesktopPlatformModule::DesktopPlatformModule()
{
   StartupModule();
}

void DesktopPlatformModule::StartupModule()
{
	m_DesktopPlatform = new DesktopPlatform();
}

void DesktopPlatformModule::ShutdownModule()
{
	if (m_DesktopPlatform)
	{
		delete m_DesktopPlatform;
		m_DesktopPlatform = nullptr;
	}
}


