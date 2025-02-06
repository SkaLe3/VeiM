#include "DesktopPlatformModule.h"
#include "Windows/DesktopPlatformWindows.h"

DesktopPlatformModule DesktopPlatformModule::m_Instance;

DesktopPlatformModule::DesktopPlatformModule()
{
   StartupModule();
}

DesktopPlatformModule::~DesktopPlatformModule()
{
	ShutdownModule();
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


