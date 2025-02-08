#include "GameClasses.h"

using namespace VeiM;

namespace
{
	ClassInfo gameCharacterInfo = {
		"GameCharacter",
		{"m_Name"},
		{"SetName", "GetName", "Attack", "Start", "Update"}
	};
	Base::Registrar<GameCharacter> registrar("GameCharacter", gameCharacterInfo);
}