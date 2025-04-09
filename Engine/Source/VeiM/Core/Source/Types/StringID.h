#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	/* NOTE:
	 * This StringID class is inspired by Jason Gregory's string interning approach in "Game Engine Architecture".
	 * The idea is to avoid repeatedly storing the same strings in memory by associating them with unique IDs.
	 * Instead of comparing full strings, we compare their IDs, which is much faster and reduces memory usage.
	 */

	// TODO: Add hardcoded names with EStringID + ID_None

	enum EFindStringID
	{
		StringID_Find,
		StringID_Add
	};


	class StringID
	{
	public:
		explicit StringID(const char* str, EFindStringID findType = StringID_Add);
		uint32 GetID() const { return m_ID; }
		String ToString();

		friend bool operator==(const StringID& lhs, const StringID& rhs)
		{
			return lhs.m_ID == rhs.m_ID;
		}

	private:
		uint32 m_ID;
	};
}