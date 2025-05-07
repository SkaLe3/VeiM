#pragma once
#include "CoreDefines.h"

#include <functional>

namespace VeiM
{
	class Object;

	// Add thread safety
	class ObjectCreatorSystem
	{
	public:
		void AddObject(Object* obj, Object* creator)
		{
			m_CreatorMap[creator].push_back(obj);
		}
		void RemoveObject(Object* obj, Object* creator)
		{
			auto& children = m_CreatorMap[creator];
			children.erase(std::remove(children.begin(), children.end(), obj), children.end());
		}
		std::vector<Object*> GetObjectsWithCreator(Object* creator)
		{
			return m_CreatorMap[creator];
		}

		void ForEachObjectWithCreator(Object* creator, std::function<bool(Object*)> func, bool includeNested = false)
		{
			std::vector<Object*> toProcess;
			{
				toProcess = m_CreatorMap[creator];
			}
			while (!toProcess.empty())
			{
				Object* current = toProcess.back();
				toProcess.pop_back();
				if (!func(current))
				{
					break;
				}
				if (includeNested)
				{
					auto children = m_CreatorMap[current];
					toProcess.insert(toProcess.end(), children.begin(), children.end());
				}
			}
		}

	private:
		std::unordered_map<Object*, std::vector<Object*>> m_CreatorMap;
	};
}