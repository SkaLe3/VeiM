#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	struct CORE_API Guid
	{
	public:
		Guid();
		Guid(uint64_t guid);
		Guid(const Guid&) = default;
		static Guid NewGuid();

		operator uint64_t() const { return m_Guid; }
	private:
		uint64 m_Guid;
	};
}

namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<VeiM::Guid>
	{
		std::size_t operator()(const VeiM::Guid guid) const
		{
			return (VeiM::uint64)guid;
		}
	};
}