#pragma once		   

#include "IDesktopPlatform.h"
#include <Windows.h>
#include <string>


#ifdef DESKTOPPLATFORM_EXPORTS
#define DESKTOPPLATFORM_API __declspec(dllexport)
#else
#define DESKTOPPLATFORM_API __declspec(dllimport)
#endif


class IDesktopPlatform;

class DESKTOPPLATFORM_API DesktopPlatformModule
{
public:
	virtual void StartupModule();
	virtual void ShutdownModule();

	static IDesktopPlatform* Get()
	{
		return m_Instance.GetSingleton();
	}
private:
	virtual IDesktopPlatform* GetSingleton() const { return m_DesktopPlatform; }

	IDesktopPlatform* m_DesktopPlatform;



	static DesktopPlatformModule m_Instance;

};