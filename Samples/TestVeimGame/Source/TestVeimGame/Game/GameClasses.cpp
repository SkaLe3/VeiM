#include "GameClasses.h"

using namespace VeiM;


IMPLEMENT_CLASS(PatapimNameComponent)
IMPLEMENT_CLASS(PatapimEntity)


void PatapimEntity::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(PatapimEntity, StringProperty, m_PatapimName);
	REGISTER_PROPERTY(PatapimEntity, BoolProperty, m_bIsAlive);
	REGISTER_PROPERTY(PatapimEntity, ObjectProperty, m_PNComponent);

}

PatapimEntity::PatapimEntity()
{
	m_PNComponent = CreateComponent<PatapimNameComponent>(StringID("DefaultComponent"));
	DefaultEntityTick.bTickable = true;
	
}

PatapimEntity::~PatapimEntity()
{
	
}

void PatapimEntity::Tick(float deltaTime)
{
	//VM_CORE_WARN("Patapim Tick");
}

void PatapimNameComponent::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(PatapimNameComponent, StringProperty, m_ComponentName);

}

PatapimNameComponent::PatapimNameComponent()
{
	DefaultComponentTick.bTickable = true;
}

PatapimNameComponent::~PatapimNameComponent()
{
	
}

void PatapimNameComponent::TickComponent(float deltaTime)
{
	//VM_CORE_INFO("Patapim Component Tick");
}
