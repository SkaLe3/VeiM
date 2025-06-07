#pragma once
#include "CoreDefines.h"
#include "Engine/CoreObject.h"
#include "Engine/Reflection.h"

namespace VeiM
{
	class Controller;

	class CORE_API Player : public Object
	{
		DECLARE_CLASS(Player, Object)
	public:
		ObjectPtr<Controller> PlayerController;
		// ObjectPtr<ViewportClient> Viewport;
	};
}