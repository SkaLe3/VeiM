#include "GameClasses.h"

#include "Engine/GameStatics.h"
#include "Engine/Controller.h"
#include "Engine/InputComponent.h"


#include "Engine/Engine.h"

using namespace VeiM;


IMPLEMENT_CLASS(PatapimNameComponent)
IMPLEMENT_CLASS(PatapimMeshComponent)
IMPLEMENT_CLASS(PatapimEntity)
IMPLEMENT_CLASS(CapucinoAgent)


void PatapimEntity::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(PatapimEntity, StringProperty, m_PatapimName);
	REGISTER_PROPERTY(PatapimEntity, BoolProperty, m_bIsAlive);
	REGISTER_PROPERTY(PatapimEntity, ObjectProperty, m_PNComponent);

}

PatapimEntity::PatapimEntity()
{
	DefaultEntityTick.bTickable = true;

	m_PNComponent = CreateComponent<PatapimNameComponent>(StringID("DefaultComponent"));
	m_Mesh = CreateComponent<PatapimMeshComponent>(StringID("PatapimMesh"));
	PatapimMeshComponent* m_Mesh2 = CreateComponent<PatapimMeshComponent>(StringID("PatapimMesh2"));

	SetRootComponent(m_Mesh);
	m_Mesh2->SetupAttachment(m_Mesh);
}

PatapimEntity::~PatapimEntity()
{

}

void PatapimEntity::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	static int32 count = 0;
	if (count % 60 == 0)
	{
		VM_CORE_WARN("Patapim Tick count: {0}", count);
	}
	count++;
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
	Super::TickComponent(deltaTime);

	static int32 count = 0;
	if (count % 60 == 0)
	{
		VM_CORE_WARN("Patapim Component Tick count: {0}", count);
	}
	count++;
}

PatapimMeshComponent::PatapimMeshComponent()
{
	DefaultComponentTick.bTickable = true;
}

PatapimMeshComponent::~PatapimMeshComponent()
{

}

void PatapimMeshComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);
	static int32 count = 0;
	if (count % 200 == 0)
	{
		VM_CORE_WARN("Patapim MeshComp Tick count: {0}", count);
	}
	count++;
}

void PatapimMeshComponent::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(PatapimMeshComponent, StringProperty, m_MeshName);

}

void CapucinoAgent::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
}

CapucinoAgent::CapucinoAgent()
{
	DefaultEntityTick.bTickable = true;

	SceneComponent* rootScene = CreateComponent<SceneComponent>(StringID("RootSceneComponent"));
	SetRootComponent(rootScene);

	m_MouseValue = Math::Utils::ZeroVector2d;

}

CapucinoAgent::~CapucinoAgent()
{

}

void CapucinoAgent::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if (m_MouseValue.x != 0 || m_MouseValue.y != 0)
	{
		g_Engine->DebugDisableEditorCamera();
		//VM_CORE_TRACE("Mouse Value: [{}, {}]", m_MouseValue.x, m_MouseValue.y);
		g_Engine->DebugEditorMouseCallback(m_MouseValue.x, m_MouseValue.y);
	}
}

void CapucinoAgent::SetupInputComponent(InputComponent* ic)
{
	Super::SetupInputComponent(ic);

	ic->BindAction(StringID("Jump"), EInputEvent::Pressed, this, &CapucinoAgent::Jump);
	ic->BindAxis(StringID("Move"), this, &CapucinoAgent::Move);
}

void CapucinoAgent::BeginPlay()
{
	Super::BeginPlay();

	if (Controller* controller = GameStatics::GetController(0))
	{
		controller->Posses(this);
	}

	InputComponent* ic = EntityInputComponent;

	if (ic)
	{
		ic->BindKey(IKey::G, EInputEvent::Pressed, this, &CapucinoAgent::OnKeyJump);
		ic->BindAxisKey(IKey::MouseX, this, &CapucinoAgent::LookYaw);
		ic->BindAxisKey(IKey::MouseY, this, &CapucinoAgent::LookPitch);
	}
	

}

void CapucinoAgent::OnKeyJump()
{
	VM_CORE_TRACE("CapucinoAgent::OnKeyJump");
}

void CapucinoAgent::Jump()
{
	VM_CORE_TRACE("Jump");
}

void CapucinoAgent::Move(float dir)
{
	VM_CORE_TRACE("Move");
}

void CapucinoAgent::LookYaw(float value)
{
	m_MouseValue.x = value;
}

void CapucinoAgent::LookPitch(float value)
{
	m_MouseValue.y = value;
}
