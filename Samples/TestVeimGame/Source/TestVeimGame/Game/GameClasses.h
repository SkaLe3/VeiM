#pragma once
#include "CoreDefines.h"
#include "TestVeimGame.h"

#include "Test/TestRenderer.h"

#include <string>
#include <vector>


#include "Engine/Entity.h"
#include "Engine/Classes/Game/Agent.h"
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

class TESTVEIMGAME_API PatapimMeshComponent : public VeiM::SceneComponent
{
	DECLARE_CLASS(PatapimMeshComponent, VeiM::SceneComponent)
public:
	PatapimMeshComponent();
	~PatapimMeshComponent();

	virtual void TickComponent(float deltaTime) override;

private:
	VeiM::String m_MeshName = "MeshP";
};



class TESTVEIMGAME_API PatapimEntity : public VeiM::Entity
{
	DECLARE_CLASS(PatapimEntity, VeiM::Entity)
public:
	PatapimEntity();
	~PatapimEntity();

	virtual void Tick(float deltaTime) override;
private:
	VeiM::String m_PatapimName = "Brr Brr Banana";
	bool m_bIsAlive = true;
	VeiM::ObjectPtr<PatapimNameComponent> m_PNComponent;
	VeiM::ObjectPtr<PatapimMeshComponent> m_Mesh;
};


class TESTVEIMGAME_API CapucinoAgent : public VeiM::Agent
{
	DECLARE_CLASS(CapucinoAgent, VeiM::Agent)
public:
	CapucinoAgent();
	~CapucinoAgent();

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

