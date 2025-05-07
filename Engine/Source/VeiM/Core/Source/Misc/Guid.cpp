#include "Guid.h"

#include <random>
#include <unordered_map>
namespace VeiM
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_RandomEngine(s_RandomDevice());
	static std::uniform_int_distribution<uint64> s_UniformDistribution;


	Guid::Guid() : m_Guid(0)
	{

	}

	Guid::Guid(uint64_t guid) : m_Guid(guid)
	{

	}

	Guid Guid::NewGuid()
	{
		return	Guid(s_UniformDistribution(s_RandomEngine));
	}

}

