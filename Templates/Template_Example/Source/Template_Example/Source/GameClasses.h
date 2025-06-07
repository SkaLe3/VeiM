#pragma once
#include "CoreDefines.h"
#include "Template_Example.h"

#include <string>
#include <vector>


#include "Engine/Entity.h"
#include "Engine/Classes/Game/Agent.h"
#include "Engine/Component.h"
#include "Engine/ObjectPtr.h"
#include "Engine/Reflection.h"


class VeiM::ClassDescriptor;

class Template_Example_API ExampleComponent : public VeiM::Component
{
	DECLARE_CLASS(ExampleComponent, VeiM::Component)
public:
	ExampleComponent();
	~ExampleComponent();

	virtual void TickComponent(float deltaTime) override;

private:
	VeiM::String m_ComponentName = "Example_Component_Name";

};

class Template_Example_API ExampleSceneComponent : public VeiM::SceneComponent
{
	DECLARE_CLASS(ExampleSceneComponent, VeiM::SceneComponent)
public:
	ExampleSceneComponent();
	~ExampleSceneComponent();

	virtual void TickComponent(float deltaTime) override;

private:
	VeiM::String m_ExampleString = "Example_String";
};



class Template_Example_API ExampleEntity : public VeiM::Entity
{
	DECLARE_CLASS(ExampleEntity, VeiM::Entity)
public:
	ExampleEntity();
	~ExampleEntity();

	virtual void Tick(float deltaTime) override;
private:
	VeiM::String m_EntityName = "Example_Entity_Name";
	bool m_ExampleBool = true;
	VeiM::ObjectPtr<ExampleComponent> m_ExComp;
	VeiM::ObjectPtr<ExampleSceneComponent> m_ExSceneComp;
};


class Template_Example_API ExampleAgent : public VeiM::Agent
{
	DECLARE_CLASS(ExampleAgent, VeiM::Agent)
public:
	ExampleAgent();
	~ExampleAgent();

	virtual void Tick(float deltaTime) override;
	virtual void SetupInputComponent(VeiM::InputComponent* ic) override;
protected:
	virtual void BeginPlay() override;

	void OnKeyJump();
	void Jump();
	void Move(float dir);
	void LookYaw(float value);
	void LookPitch(float value);

private:
	glm::vec2 m_MouseValue;

};

