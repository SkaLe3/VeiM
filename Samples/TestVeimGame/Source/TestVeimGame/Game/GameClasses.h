#pragma once
#include "CoreDefines.h"
#include "Test/TestRenderer.h"

#include <string>
#include <vector>

extern "C"
{
	__declspec(dllexport) void gameLog();
}


#include "Test/Base.h"


class GameCharacter : public VeiM::Base
{
public:
	static VeiM::UniquePtr<VeiM::Base> Create()
	{
		return VeiM::MakeUnique<GameCharacter>();
	}
	virtual void print() const override {
		VM_WARN("This is a GameCharacter object");
	}
	virtual void Start() override
	{
		SetName("Hero");
	}
	virtual void Update(float deltaTime) override
	{
		Attack();
	}
	void SetName(const VeiM::String& name)
	{
		this->m_Name = name;
		VM_WARN("New character name was set to: {0}", name);
	}
	VeiM::String GetName() const
	{
		return m_Name;
	}
	void Attack()
	{
		if (m_Name == "Hero")
		{
			VM_WARN("GameCharacter {0} is attacking!", m_Name.c_str());
			SetName("Not Hero");
		}
	}

private:
	VeiM::String m_Name;
};




