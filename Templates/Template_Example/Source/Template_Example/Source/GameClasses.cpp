#include "GameClasses.h"

#include "Engine/GameStatics.h"
#include "Engine/Controller.h"
#include "Engine/InputComponent.h"


#include "Engine/Engine.h"

using namespace VeiM;


IMPLEMENT_CLASS(ExampleComponent)
IMPLEMENT_CLASS(ExampleSceneComponent)
IMPLEMENT_CLASS(ExampleEntity)
IMPLEMENT_CLASS(ExampleAgent)


void ExampleEntity::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(ExampleEntity, StringProperty, m_EntityName);
	REGISTER_PROPERTY(ExampleEntity, BoolProperty, m_ExampleBool);
	REGISTER_PROPERTY(ExampleEntity, ObjectProperty, m_ExComp);
	REGISTER_PROPERTY(ExampleEntity, ObjectProperty, m_ExSceneComp);

}

ExampleEntity::ExampleEntity()
{
	DefaultEntityTick.bTickable = true;

	m_ExComp = CreateComponent<ExampleComponent>(StringID("DefaultComponent"));
	m_ExSceneComp = CreateComponent<ExampleSceneComponent>(StringID("ExampleSceneComponent"));
	ExampleSceneComponent* sceneComp2 = CreateComponent<ExampleSceneComponent>(StringID("ExampleSceneComponent2"));

	SetRootComponent(m_ExSceneComp);
	sceneComp2->SetupAttachment(m_ExSceneComp);
}

ExampleEntity::~ExampleEntity()
{

}

void ExampleEntity::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	static int32 count = 0;
	if (count % 600 == 0)
	{
		VM_CORE_WARN("Entity Tick count: {0}", count);
	}
	count++;
}

void ExampleComponent::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(ExampleComponent, StringProperty, m_ComponentName);

}

ExampleComponent::ExampleComponent()
{
	DefaultComponentTick.bTickable = true;
}

ExampleComponent::~ExampleComponent()
{

}

void ExampleComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);

	static int32 count = 0;
	if (count % 700 == 0)
	{
		VM_CORE_WARN("Component Tick count: {0}", count);
	}
	count++;
}

ExampleSceneComponent::ExampleSceneComponent()
{
	DefaultComponentTick.bTickable = true;
}

ExampleSceneComponent::~ExampleSceneComponent()
{

}

void ExampleSceneComponent::TickComponent(float deltaTime)
{
	Super::TickComponent(deltaTime);
	static int32 count = 0;
	if (count % 800 == 0)
	{
		VM_CORE_WARN("Scene Component Tick count: {0}", count);
	}
	count++;
}

void ExampleSceneComponent::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
	REGISTER_PROPERTY(ExampleSceneComponent, StringProperty, m_ExampleString);

}

void ExampleAgent::RegisterProperties(ClassDescriptor* classDesc)
{
	Super::RegisterProperties(classDesc);
}

ExampleAgent::ExampleAgent()
{
	DefaultEntityTick.bTickable = true;

	SceneComponent* rootScene = CreateComponent<SceneComponent>(StringID("RootSceneComponent"));
	SetRootComponent(rootScene);

	m_MouseValue = Math::Utils::ZeroVector2d;

}

ExampleAgent::~ExampleAgent()
{

}

void ExampleAgent::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if (m_MouseValue.x != 0 || m_MouseValue.y != 0)
	{
		g_Engine->DebugDisableEditorCamera();
		g_Engine->DebugEditorMouseCallback(m_MouseValue.x, m_MouseValue.y);
	}
}

void ExampleAgent::SetupInputComponent(InputComponent* ic)
{
	Super::SetupInputComponent(ic);

	ic->BindAction(StringID("Jump"), EInputEvent::Pressed, this, &ExampleAgent::Jump);
	ic->BindAxis(StringID("Move"), this, &ExampleAgent::Move);
}

void ExampleAgent::BeginPlay()
{
	Super::BeginPlay();

	if (Controller* controller = GameStatics::GetController(0))
	{
		controller->Posses(this);
	}

	InputComponent* ic = EntityInputComponent;

	if (ic)
	{
		ic->BindKey(IKey::G, EInputEvent::Pressed, this, &ExampleAgent::OnKeyJump);
		ic->BindAxisKey(IKey::MouseX, this, &ExampleAgent::LookYaw);
		ic->BindAxisKey(IKey::MouseY, this, &ExampleAgent::LookPitch);
	}
	

}

void ExampleAgent::OnKeyJump()
{
	VM_CORE_TRACE("ExampleAgent::OnKeyJump");
}

void ExampleAgent::Jump()
{
	VM_CORE_TRACE("Jump");
}

void ExampleAgent::Move(float dir)
{
	VM_CORE_TRACE("Move");
}

void ExampleAgent::LookYaw(float value)
{
	m_MouseValue.x = value;
}

void ExampleAgent::LookPitch(float value)
{
	m_MouseValue.y = value;
}
