#pragma once
#include <tchar.h>

namespace VeiM
{
	template<typename T>
	struct BaseProcessHandle
	{
	public:
		BaseProcessHandle() : m_Handle(0) {}
		BaseProcessHandle(T handle) : m_Handle(handle) {}
		T Get() const { return m_Handle; }
		bool IsValid() const { return m_Handle != 0; }
		void Invalidate() { m_Handle = 0; }
	protected:
		T m_Handle;
	};

	struct GenericPlatformService
	{
	public:
		static const TCHAR* BaseDir();
	};
}