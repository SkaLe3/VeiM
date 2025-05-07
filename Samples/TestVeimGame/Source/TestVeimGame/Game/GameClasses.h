#pragma once
#include "CoreDefines.h"
#include "TestVeimGame.h"

#include "Test/TestRenderer.h"

#include <string>
#include <vector>


#include "Engine/Entity.h"
#include "Engine/Component.h"
#include "Engine/ObjectPtr.h"
#include "Engine/Reflection.h"


class VeiM::ClassDescriptor;

class TESTVEIMGAME_API PatapimNameComponent : public VeiM::Component
{
	DECLARE_CLASS(PatapimNameComponent, VeiM::Component)
public:
	PatapimNameComponent();
	~PatapimNameComponent();

	virtual void TickComponent(float deltaTime) override;

private:
	VeiM::String m_ComponentName = "Brr Brr Patapim Component";

};


class TESTVEIMGAME_API PatapimEntity : public VeiM::Entity
{
	DECLARE_CLASS(PatapimEntity, VeiM::Entity)
public:
	PatapimEntity();
	~PatapimEntity();

	virtual void Tick(float deltaTime) override;
private:
	VeiM::String m_PatapimName = "Brr Brr Patapim";
	bool m_bIsAlive = true;
	VeiM::ObjectPtr<PatapimNameComponent> m_PNComponent;
};

