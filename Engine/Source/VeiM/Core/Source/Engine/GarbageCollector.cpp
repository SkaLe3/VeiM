#include "GarbageCollector.h"
#include "Engine/CoreObject.h"
#include "Engine/ObjectCreatorSystem.h"
#include "Engine/WeakObjectPtr.h"
#include "Engine/Reflection.h"
#include "Engine/Engine.h"

namespace VeiM
{
	// Static instance initialization
	GarbageCollector* GarbageCollector::s_Instance = nullptr;

	void PointerRegistry::RegisterStrongPtr(Object* obj, void* ptrLocation)
	{
		if (obj) {
			s_StrongPtrLocations[obj].insert(ptrLocation);
		}
	}

	void PointerRegistry::UnregisterStrongPtr(Object* obj, void* ptrLocation)
	{
		if (obj) {
			auto it = s_StrongPtrLocations.find(obj);
			if (it != s_StrongPtrLocations.end()) {
				it->second.erase(ptrLocation);
				if (it->second.empty()) {
					s_StrongPtrLocations.erase(it);
				}
			}
		}
	}

	void PointerRegistry::RegisterWeakPtr(Object* obj, void* ptrLocation)
	{
		if (obj) {
			s_WeakPtrLocations[obj].insert(ptrLocation);
		}
	}

	void PointerRegistry::UnregisterWeakPtr(Object* obj, void* ptrLocation)
	{
		if (obj) {
			auto it = s_WeakPtrLocations.find(obj);
			if (it != s_WeakPtrLocations.end()) {
				it->second.erase(ptrLocation);
				if (it->second.empty()) {
					s_WeakPtrLocations.erase(it);
				}
			}
		}
	}

	// PointerRegistry static member initialization
	std::unordered_map<Object*, std::unordered_set<void*>> PointerRegistry::s_StrongPtrLocations;
	std::unordered_map<Object*, std::unordered_set<void*>> PointerRegistry::s_WeakPtrLocations;

	GarbageCollector& GarbageCollector::Get()
	{
		if (!s_Instance) 
		{
			VM_CORE_TRACE("[GC] Garbage Collector Created");
			s_Instance = new GarbageCollector();
			s_Instance->m_LastCollectionTime = std::chrono::steady_clock::now();
		}
		return *s_Instance;
	}


	void GarbageCollector::CollectGarbage(bool bForce)
	{
		if (!bForce && !ShouldCollect()) {
			return;
		}

		PrepareCollection();
		MarkPhase();
		SweepPhase();
		FinalizeCollection();

		m_LastCollectionTime = std::chrono::steady_clock::now();
		g_Engine->NotifyGCRunThisFrame();
	}

	void GarbageCollector::MarkPhase()
	{
		MarkRoots();
	}

	void GarbageCollector::MarkRoots()
	{
		for (auto* obj : m_RootObjects) {
			if (!obj->IsPendingKill()) {
				ProcessMark(obj);
			}
		}
	}


	void GarbageCollector::ProcessMark(Object* obj)
	{
		if (!obj || obj->m_bIsMarkedForGC || obj->IsPendingKill()) {
			return;
		}
		obj->m_bIsMarkedForGC = true;
		obj->MarkReferencedObjects(*this);
	}

	void GarbageCollector::MarkReachable(Object* obj)
	{
		if (obj && !obj->m_bIsMarkedForGC && !obj->IsPendingKill()) {
			ProcessMark(obj);
		}
	}

	uint32 GarbageCollector::GetStrongPtrRegisteredCount()
	{
		return PointerRegistry::s_StrongPtrLocations.size();
	}

	uint32 GarbageCollector::GetWeakPtrRegisteredCount()
	{
		return PointerRegistry::s_WeakPtrLocations.size();
	}

	std::unordered_set<Object*>& GarbageCollector::Debug_GetAllObjects()
	{
		return m_Objects;
	}

	void GarbageCollector::SweepPhase()
	{
		for (auto* obj : m_Objects) {
			if (obj->IsPendingKill())
			{
				m_PendingKillObjects.push_back(obj);
			}
			else if (!obj->m_bIsMarkedForGC && !obj->IsPendingKill() && !obj->IsRoot() && !obj->HasFlag(Object::FLAG_INITIALIZING)) 
			{

				obj->MarkPendingKill();
				obj->StartDestroy();
				m_PendingKillObjects.push_back(obj);

				// Invalidate any weak pointers to this object
				auto weakIt = PointerRegistry::s_WeakPtrLocations.find(obj);
				if (weakIt != PointerRegistry::s_WeakPtrLocations.end()) 
				{
					for (void* ptrLoc : weakIt->second) 
					{
						// Call Invalidate() on each weak pointer
						auto** ptrToWeakPtr = static_cast<void**>(ptrLoc);
						if (*ptrToWeakPtr) 
						{
							// This is a bit hacky - need to use a type-safe approach,
							reinterpret_cast<WeakObjectPtr<Object>*>(*ptrToWeakPtr)->Invalidate();
						}
					}
				}

				// Invalidate any strong pointers to this object
				auto stronIt = PointerRegistry::s_StrongPtrLocations.find(obj);
				if (stronIt != PointerRegistry::s_StrongPtrLocations.end()) 
				{
					for (void* ptrLoc : stronIt->second) 
					{
						// Call Invalidate() on each weak pointer
						auto** ptrToStrongPtr = static_cast<void**>(ptrLoc);
						if (*ptrToStrongPtr) 
						{
							// This is a bit hacky - need to use a type-safe approach,
							reinterpret_cast<ObjectPtr<Object>*>(ptrToStrongPtr)->Invalidate();
						}
					}
				}
			}
		}
	}

	void GarbageCollector::PrepareCollection()
	{
		// Reset marking state
		for (auto* obj : m_Objects) 
		{
			obj->m_bIsMarkedForGC = false;
		}
	}

	void GarbageCollector::FinalizeCollection()
	{
		// Process pending kill objects. This should be deferred
		// to a separate thread or spread across multiple frames


		const size_t MAX_OBJECTS_PER_FRAME = 100; // Temporary local. Move to config
		size_t processedCount = 0;
		static bool bFlagNewBatch = true;

		while (processedCount < MAX_OBJECTS_PER_FRAME && !m_PendingKillObjects.empty()) 
		{
			Object* obj = m_PendingKillObjects.back();
			m_PendingKillObjects.pop_back();
			VM_CORE_TRACE("[GC] Collecting object of class: '{0}'", obj->GetClass()->Name.Get());
			obj->ClearFlag(Object::FLAG_PENDING_KILL);
			obj->SetFlag(Object::FLAG_GARBAGE);
			obj->FinishDestroy();
			obj->GetCreatorSystem().RemoveObject(obj, obj->GetCreator());
			delete obj; // This will call UnregisterObject through Object destructor
			processedCount++;
		}
		if (bFlagNewBatch)
		{
			m_LastCollectedCount = processedCount;
			bFlagNewBatch = false;
		}
		else
		{
			m_LastCollectedCount += processedCount;
		}

		if (m_PendingKillObjects.empty())
		{
			bFlagNewBatch = true;
			if (m_LastCollectedCount != 0) m_LastCollectedNonZeroCount = m_LastCollectedCount;
			VM_CORE_TRACE("[GC] Collected {0} objects", std::to_string(m_LastCollectedCount));
		}
	}

	bool GarbageCollector::ShouldCollect() const
	{
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_LastCollectionTime).count();

		if (elapsed >= m_AutomaticCollectionIntervalSeconds)
		{
			return true;
		}
		const size_t totalObjects = m_Objects.size();
		const size_t deadObjects = m_PendingKillObjects.size();

		// If we have a significant number of dead objects, collect
		if (deadObjects > 0 && deadObjects >= m_CollectionThreshold)
		{
			return true;
		}
		return false;
	}

	void GarbageCollector::RegisterObject(Object* obj)
	{
		if (!obj) return;
		m_Objects.insert(obj);
		m_TotalObjectCount++;

		// Consider performing collection asynchronously or deferring it
		// to avoid interrupting gameplay during object creation spikes
		if (ShouldCollect())
			CollectGarbage(false);
	}

	void GarbageCollector::UnregisterObject(Object* obj)
	{
		if (!obj) return;

		m_Objects.erase(obj);
		m_RootObjects.erase(obj);

		// Remove from pending kill list if present
		auto it = std::find(m_PendingKillObjects.begin(), m_PendingKillObjects.end(), obj);
		if (it != m_PendingKillObjects.end()) {
			m_PendingKillObjects.erase(it);
		}

		// Clean up pointer registries
		auto strongIt = PointerRegistry::s_StrongPtrLocations.find(obj);
		if (strongIt != PointerRegistry::s_StrongPtrLocations.end()) {
			PointerRegistry::s_StrongPtrLocations.erase(strongIt);
		}

		auto weakIt = PointerRegistry::s_WeakPtrLocations.find(obj);
		if (weakIt != PointerRegistry::s_WeakPtrLocations.end()) {
			PointerRegistry::s_WeakPtrLocations.erase(weakIt);
		}
	}

	void GarbageCollector::AddRoot(Object* obj)
	{
		if (!obj) return;
		obj->SetFlag(Object::FLAG_ROOT);
		m_RootObjects.insert(obj);
	}

	void GarbageCollector::RemoveRoot(Object* obj)
	{
		if (!obj) return;
		obj->ClearFlag(Object::FLAG_ROOT);
		m_RootObjects.erase(obj);
	}

	GarbageCollector::~GarbageCollector()	{}

}

