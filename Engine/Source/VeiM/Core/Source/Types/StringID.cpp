#include "StringID.h"
#include <unordered_map>
#include <functional>


namespace VeiM
{
	static std::unordered_map<uint32, const char*> g_StringIdTable;

	static std::unordered_map<uint32, const char*>& GetStringIdTable()
	{
		return g_StringIdTable;
	}




	StringID::StringID(const char* str)
	{
		uint32 sid = static_cast<uint32>(std::hash<const char*>{}(str));

		auto it = GetStringIdTable().find(sid);
		if (it != GetStringIdTable().end())
		{
			m_ID = it->first;
		}
		else
		{
			m_ID = sid;
			GetStringIdTable()[m_ID] = strdup(str);
		}
	}

	String StringID::ToString()
	{
		String out;
		if (auto it = GetStringIdTable().find(m_ID); it != GetStringIdTable().end())
		{
			out = it->second;
		}
		return out;
	}

}

