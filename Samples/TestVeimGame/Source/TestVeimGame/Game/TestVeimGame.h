#pragma once
#include "VeiMCore.h"

#if IS_UNIFIED
	#define TESTVEIMGAME_API
#else
	#ifdef _WIN32
		#ifdef GAME_PRIMARY_EXPORTS
			#define TESTVEIMGAME_API __declspec(dllexport)
		#else
			#define TESTVEIMGAME_API __declspec(dllimport)
		#endif
	#else
		#define TESTVEIMGAME_API
	#endif
#endif