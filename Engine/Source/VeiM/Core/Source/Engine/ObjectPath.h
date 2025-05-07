#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class CORE_API ObjectPath
	{
	public:
		ObjectPath() = default;
		ObjectPath(const char* path) : m_Path(path) {}
		ObjectPath(const String& path) : m_Path(path) {}
		ObjectPath(const fs::path& path) : m_Path(path) {}

		const fs::path& GetPath() const { return m_Path; }
		bool IsValid() const { return !m_Path.empty(); }

		bool operator==(const ObjectPath& other) const { return m_Path == other.m_Path; }
		bool operator!=(const ObjectPath& other) const { return m_Path != other.m_Path; }

		struct Hash
		{
			size_t operator()(const ObjectPath& path) const
			{
				return std::hash<fs::path>()(path.m_Path);
			}
		};

	private:
		fs::path m_Path;
	};
}