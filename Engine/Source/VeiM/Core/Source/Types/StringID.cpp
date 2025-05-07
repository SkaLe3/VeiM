#include "StringID.h"
#include <unordered_map>
#include <functional>
#include "CityHash/city.h"


namespace VeiM
{
	static std::unordered_map<uint32, const char*> g_StringIdTable;

	static std::unordered_map<uint32, const char*>& GetStringIdTable()
	{
		return g_StringIdTable;
	}

	bool StringID::s_CommonStringsRegistered = false;

	StringID::StringID(const char* str, EFindStringID findType /*= StringID_Add*/)
	{
		size_t strLength = strlen(str);
		uint32 hash = CityHash32(str, strLength);

		auto it = GetStringIdTable().find(hash);
		if (it != GetStringIdTable().end())
		{
			m_ID = it->first;
		}
		else
		{
			if (findType == EFindStringID::StringID_Add)
			{
				m_ID = hash;
				GetStringIdTable()[m_ID] = _strdup(str);
			}
			else
			{
				m_ID = FromEStringID(EStringID::None);
			}
		}
	}

	StringID::StringID()
	{
		m_ID = FromEStringID(EStringID::None);
	}

	StringID::StringID(EStringID eStrID)
	{
		m_ID = (eStrID == EStringID::None ? 0 : FromEStringID(eStrID));
	}

	const char* StringID::Get() const
	{
		if (auto it = GetStringIdTable().find(m_ID); it != GetStringIdTable().end())
			return it->second;
		return "";
	}

	String StringID::ToString() const
	{
		String out;
		if (auto it = GetStringIdTable().find(m_ID); it != GetStringIdTable().end())
		{
			out = it->second;
		}
		return out;
	}

	EStringID StringID::ToEStringID() const
	{
		if (m_ID >= static_cast<uint32>(EStringID::MaxStringIdIdx))
			return EStringID::None;
		return static_cast<EStringID>(m_ID);
	}

	bool StringID::RegisterCommonStrings()
	{
		if (s_CommonStringsRegistered) return true;
#define REGISTER_STRINGID(val, sid) \
	GetStringIdTable()[static_cast<uint32>(EStringID::sid)] = #sid;
#include "StringIDCommon.inl"
#undef REGISTER_STRINGID
		s_CommonStringsRegistered = true;
		return true;
	}

	uint32 StringID::FromEStringID(EStringID eStrID)
	{
		return static_cast<uint32>(eStrID);
	}

}

