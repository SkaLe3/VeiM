#pragma once
#include "CoreDefines.h"
#include "Engine/Entity.h"

namespace VeiM
{
	class CORE_API ServiceEntity : public Entity
	{
		DECLARE_CLASS(ServiceEntity, Entity)
	public:
		/* Called after all begin play */
		virtual void StartPlay() {}
	private:
	};
}