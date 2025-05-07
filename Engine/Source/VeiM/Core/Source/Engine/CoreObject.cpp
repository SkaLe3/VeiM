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
		ClearFlag(FLAG_INITIALIZING);
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
			ownerEntity->AddOwnedComponent(CastObject<Component>(createdComponent));
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

	Object* Object::CreateComponent_Internal(Object* owner, StringID name, ClassDescriptor* instanceType)
	{
		return nullptr;
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

