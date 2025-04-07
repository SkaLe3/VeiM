#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class StringID
	{
	public:
		explicit StringID(const char* str);
		explicit StringID(const char* str, int a);
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