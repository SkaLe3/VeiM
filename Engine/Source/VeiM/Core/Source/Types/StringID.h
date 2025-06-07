#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	/* NOTE:
	 * This StringID class is inspired by Jason Gregory's string interning approach in "Game Engine Architecture".
	 * The idea is to avoid repeatedly storing the same strings in memory by associating them with unique IDs.
	 * Instead of comparing full strings, we compare their IDs, which is much faster and reduces memory usage.
	 */

	// TODO: Add thread safety

#define REGISTER_STRINGID(val, sid) sid = val,
	enum class EStringID : uint32
	{
		#include "StringIDCommon.inl"
		MaxStringIdIdx
	};
#undef REGISTER_STRINGID
	enum EFindStringID
	{
		StringID_Find,
		StringID_Add
	};


	class CORE_API StringID
	{
	public:
		StringID();
		explicit StringID(const char* str, EFindStringID findType = StringID_Add);
		StringID(EStringID eStrID);
		uint32 GetID() const { return m_ID; }
		const char* Get() const;
		String ToString() const;

		EStringID ToEStringID() const;
		bool IsNone() const
		{
			return m_ID == static_cast<uint32>(EStringID::None);
		}

		friend bool operator==(const StringID& lhs, const StringID& rhs)
		{
			return lhs.m_ID == rhs.m_ID;
		}

		friend FORCEINLINE bool operator==(StringID strID, EStringID eStrID)
		{
			return strID == StringID(eStrID);
		}
		friend FORCEINLINE bool operator==(EStringID eStrID, StringID strID)
		{
			return strID == StringID(eStrID);
		}

		static bool RegisterCommonStrings();
	private:
		uint32 FromEStringID(EStringID eStrID);
	private:
		uint32 m_ID;

		static bool s_CommonStringsRegistered;
	};


	struct StringIDComparator {
		bool operator()(const StringID& a, const StringID& b) const {
			return a.ToString() < b.ToString();
		}
	};
}

namespace std 
{
	template<>
	struct hash<VeiM::StringID> {
		size_t operator()(const VeiM::StringID& sid) const noexcept {
			return static_cast<size_t>(sid.GetID());
		}
	};
}