#pragma once
#include "CoreDefines.h"

namespace VeiM
{

	class ObjectPtrBase {
	protected:
		ObjectPtrBase() = default;
		virtual ~ObjectPtrBase() = default;

		virtual void RegisterWithGC() = 0;
		virtual void UnregisterWithGC() = 0;

		virtual void* GetRawObjectPtr() const = 0;
	};
}