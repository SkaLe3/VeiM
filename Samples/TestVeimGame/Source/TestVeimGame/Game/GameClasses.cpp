#include "GameClasses.h"

using namespace VeiM;



__declspec(dllexport) void gameLog()
{
	VM_WARN("gameLog called");

}


namespace
{
	ClassInfo gameCharacterInfo = {
		"GameCharacter",
		{"Name"},
		{"SetName", "GetName", "Attack", "Start", "Update"}
	};
	Base::Registrar<GameCharacter> registrar("GameCharacter", gameCharacterInfo);
}