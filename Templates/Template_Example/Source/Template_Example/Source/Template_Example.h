#pragma once

#include "VeiMCore.h"

#if IS_UNIFIED
	#define Template_Example_API
#else
	#ifdef _WIN32
		#ifdef GAME_PRIMARY_EXPORTS
			#define Template_Example_API __declspec(dllexport)
		#else
			#define Template_Example_API __declspec(dllimport)
		#endif
	#else
		#define Template_Example_API
	#endif
#endif