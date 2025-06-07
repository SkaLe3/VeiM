#pragma once

#include "VeiMCore.h"

#if IS_UNIFIED
	#define Template_Blank_API
#else
	#ifdef _WIN32
		#ifdef GAME_PRIMARY_EXPORTS
			#define Template_Blank_API __declspec(dllexport)
		#else
			#define Template_Blank_API __declspec(dllimport)
		#endif
	#else
		#define Template_Blank_API
	#endif
#endif