#include "CoreObject.h"
#include "Engine/Reflection.h"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Engine/CoreObjectStatics.h"

namespace VeiM
{

	Object::Object()
	{
		SetFlag(FLAG_INITIALIZING);
		GarbageCollector::Get().RegisterObject(this);
	}


	Object::~Object()
	{
		if (!IsPendingKill() && HasFlag(FLAG_GARBAGE)) {
			GarbageCollector::Get().UnregisterObject(this);
		}
	}

	Object* Object::CreateComponent(StringID name, ClassDescriptor* instanceType/*, bool bTransient = false */)
	{
		// Make error when called outside of constructor

		Object* createdComponent = NewObject<Object>(instanceType, this, name);
		Entity* ownerEntity = CastObject<Entity>(this);
		if (ownerEntity)
		{
			Component* component = CastObject<Component>(createdComponent);
			ownerEntity->AddOwnedComponent(component);
		}

		return createdComponent;
	}

	void Object::SetCreator(Object* newCreator)
	{
		if (m_Creator)
		{
			GetCreatorSystem().RemoveObject(this, m_Creator.Get());
		}
		m_Creator = newCreator;	
		if (m_Creator)
		{
			GetCreatorSystem().AddObject(this, m_Creator.Get());
		}
	}

	Object* Object::GetCreatorAs(ClassDescriptor* targetClass) const
	{
		Object* creator = nullptr;
		for (Object* nextCreator = GetCreator(); creator == nullptr && nextCreator != nullptr; nextCreator->GetCreator())
		{
			if (nextCreator->IsA(targetClass))
			{
				creator = nextCreator;
			}
		}
		return creator;
	}

	bool Object::IsA(ClassDescriptor* baseClass) const
	{
		if (GetClass())
		{
			return GetClass()->IsChildOf(baseClass);
		}
		return false;
	}

	World* Object::GetWorld() const
	{
		if (Object* creator = GetCreator())
		{
			return creator->GetWorld();
		}
		return g_World;
	}

	ClassDescriptor* Object::StaticClass()
	{
		static ClassDescriptor* descriptor = nullptr;
		if (!descriptor)
		{
			descriptor = new ClassDescriptor();
			descriptor->Name = StringID("Object");
			descriptor->ParentClass = nullptr;
			descriptor->ConstructorFunc = []() -> Object* { return new Object(); };
			ClassRegistry::RegisterClass(descriptor);
			VM_CORE_INFO("Registered Object Class");
		}
		return descriptor;
	}

	void Object::SetAsAsset(bool isAsset)
	{
		if (isAsset)
			SetFlag(FLAG_ASSET);
		else
			ClearFlag(FLAG_ASSET);
	}

	void Object::MarkReferencedObjects(GarbageCollector& gc)
	{
		const auto* classDesc = GetClass();
		while (classDesc)
		{
			for (auto& [propName, prop] : classDesc->Properties)
			{
				if (prop.MarkReferencedObjects)
				{
					prop.MarkReferencedObjects(this, gc);
				}
			}
			classDesc = classDesc->ParentClass;
		}
	}


	bool Object::IsPrototype(Flags flag /*= FLAG_PROTOTYPE*/) const
	{
		for (const Object* creator = this; creator; creator = creator->GetCreator())
		{
			if (creator->HasFlag(flag))
				return true;
		}
		return false;
	}

	void Object::MarkAsRoot()
	{
		if (!IsRoot())
		{
			GarbageCollector::Get().AddRoot(this);
		}
	}

	void Object::UnmarkAsRoot()
	{
		if (IsRoot())
		{
			GarbageCollector::Get().RemoveRoot(this);
		}
	}
		
	void Object::MarkPendingKill()
	{
		if (!IsPendingKill())
		{
			SetFlag(FLAG_PENDING_KILL);
			OnMarkPendingKill();

		}
	}

	Object* Object::DuplicateObject(Object* sourceObj, Object* objCreator, StringID objName, ClassDescriptor* objClass)
	{
		if (!sourceObj || !objClass)
		{
			VM_CORE_ERROR("[Object] Failed to duplicate object: Invalid source or class");
			return nullptr;
		}
		if (GetClass() != objClass)
		{
			VM_CORE_ERROR("[Object] DuplicateObject type mismatch: Caller '{0}' - source Object '{1}'", GetClass()->Name.ToString(), objClass->Name.ToString());
			return nullptr;
		}


		Object* duplicateObj;
		duplicateObj = ClassRegistry::FindClass(objClass->Name)->ConstructorFunc();
		duplicateObj->SetCreator(objCreator);
		duplicateObj->m_Name = objName;
		duplicateObj->SetFlags((Object::Flags)sourceObj->GetFlags() & (Object::Flags::FLAG_ALL & ~Object::Flags::FLAG_ROOT));

		VM_CORE_TRACE("[Object] Duplicated Object of class: '{0}'. Creator class: '{1}'", objClass->Name.Get(), (objCreator != nullptr ? objCreator->GetClass()->Name.Get() : "nullptr"));

		CopyProperties(sourceObj, duplicateObj);
		duplicateObj->MarkAsRoot();
		Duplicate(sourceObj, duplicateObj);
		duplicateObj->UnmarkAsRoot();
		return duplicateObj;
	}

	Object* Object::CreateComponent_Internal(Object* owner, StringID name, ClassDescriptor* instanceType)
	{
		return nullptr;
	}

	void Object::CopyProperties(Object* sourceObj, Object* destObj)
	{
		if (!sourceObj || !destObj || sourceObj->GetClass() != destObj->GetClass())
		{
			VM_CORE_WARN("[Object] Cannot copy properties: Invalid objects or mismatched classes");
			return;
		}
		const auto* classDesc = sourceObj->GetClass();
		while (classDesc)
		{
			for (auto& [propName, prop] : classDesc->Properties)
			{
				if (prop.Type == EStringID::ObjectProperty || prop.Type == EStringID::SoftObjectProperty)
				{
					// These will be handled in a post-duplication phase
					continue;
				}
				else if (!prop.IsContainerType())
				{
					prop.CopyValue(sourceObj, destObj);
				}
				else
				{
					// TODO: Add containers
				}
				
			}
			classDesc = classDesc->ParentClass;
		}
	}

#if 0
	void Object::Serialize(Serializer& serializer)
	{
		// Don't serisalize transient
		const auto* classDesc = GetClass();
		while (classDesc)
		{
			for (auto& [propName, prop] : classDesc->Properties) 
			{
				// Use automatic type resolvinig
				switch (prop.Type)
				{
				case PropertyType::Bool:
					serializer.SerializeProperty(prop.Name.ToString(), prop.GetValue<bool>(this)); 
					break;
					// ... Other types
				}
			}
			classDesc = classDesc->ParentClass
		}
	}
#endif
}

