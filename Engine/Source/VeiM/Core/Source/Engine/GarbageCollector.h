#pragma once
#include "CoreDefines.h"

#include <unordered_set>
#include <vector>

namespace VeiM
{
	class Object;
	class GarbageCollector;



	class CORE_API PointerRegistry
	{
	public:
		static void RegisterStrongPtr(Object* obj, void* ptrLocation);
		static void UnregisterStrongPtr(Object* obj, void* ptrLocation);

		static void RegisterWeakPtr(Object* obj, void* ptrLocation);
		static void UnregisterWeakPtr(Object* obj, void* ptrLocation);

	private:
		// Maps objects to sets of pointer locations
		static std::unordered_map<Object*, std::unordered_set<void*>> s_StrongPtrLocations;
		static std::unordered_map<Object*, std::unordered_set<void*>> s_WeakPtrLocations;

		friend class GarbageCollector;
	};


	class CORE_API GarbageCollector
	{
	public:
		static GarbageCollector& Get();

		void CollectGarbage(bool bForce = false);

		void RegisterObject(Object* obj);
		void UnregisterObject(Object* obj); // Called by Object Destructor
		void AddRoot(Object* obj);
		void RemoveRoot(Object* obj);

		void MarkReachable(Object* obj);


		void SetCollectionThreshold(size_t min) { m_CollectionThreshold = min; }
		void SetAutomaticCollectionInterval(float seconds) { m_AutomaticCollectionIntervalSeconds = seconds; }

		uint32 GetObjectsCount() { return m_Objects.size(); }
		uint32 GetRootObjectsCount() { return m_RootObjects.size(); }
		uint32 GetLastCollectedCount() { return m_LastCollectedNonZeroCount; }
		uint32 GetStrongPtrRegisteredCount();
		uint32 GetWeakPtrRegisteredCount();


	private:
		void MarkPhase();
		void SweepPhase();

		void MarkRoots();

		void ProcessMark(Object* obj);

		void PrepareCollection();
		void FinalizeCollection();

		bool ShouldCollect() const;

	public:
		template <typename T>
		void MarkReachableArray(const std::vector<T>& objects);

		template <typename T>
		void MarkReachableSet(const std::unordered_set<T>& objects);

	private:
		GarbageCollector() = default;
		~GarbageCollector();

	private:
		std::unordered_set<Object*> m_Objects;
		std::unordered_set<Object*> m_RootObjects;
		std::vector<Object*> m_PendingKillObjects;

		size_t m_TotalObjectCount = 0;
		size_t m_LastCollectedCount = 0;
		size_t m_LastCollectedNonZeroCount = 0;
		size_t m_CollectionThreshold = 100;

		std::chrono::time_point<std::chrono::steady_clock> m_LastCollectionTime;
		float m_AutomaticCollectionIntervalSeconds = 30.0f;

		static GarbageCollector* s_Instance;
	};

	template <typename T>
	void VeiM::GarbageCollector::MarkReachableArray(const std::vector<T>& objects)
	{
		for (const T& obj : objects)
		{
			MarkReachable(obj.Get());
		}
	}

	template <typename T>
	void VeiM::GarbageCollector::MarkReachableSet(const std::unordered_set<T>& objects)
	{
		for (const T& obj : objects)
		{
			MarkReachable(obj.Get());
		}
	}

}