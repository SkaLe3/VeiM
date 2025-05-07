#pragma once
#include "Engine/CoreObject.h"
#include "Engine/Reflection.h"

namespace VeiM
{
	template <typename T>
	concept IsEngineObject = std::is_base_of_v<Object, T>;

	template <typename To, typename From> requires IsEngineObject<To>&& IsEngineObject<From>
	To* CastObject(From* src)
	{
		if (src == nullptr)
			return nullptr;

		static ClassDescriptor* toClass = To::StaticClass();

		if (src->IsA(toClass))
		{
			return static_cast<To*>(src);
		}
		return nullptr;
	}

	template<typename T>
	T* NewObject(ClassDescriptor* objClass, Object* creator, StringID name, Object::Flags flags = Object::FLAG_None)
	{
		Object* newObject = ClassRegistry::FindClass(objClass->Name)->ConstructorFunc();
		newObject->SetCreator(creator);
		newObject->m_Name = name;
		newObject->SetFlag(flags);
		VM_CORE_TRACE("[Object] Created new Object of class: '{0}'. Creator class: '{1}'", objClass->Name.Get(), (creator != nullptr ? creator->GetClass()->Name.Get() : "nullptr"));
		return static_cast<T*>(newObject);
	}
	template<typename T>
	T* NewObject(Object* creator, StringID name, Object::Flags flags = Object::FLAG_None)
	{
		return NewObject<T>(T::StaticClass(), creator, name, flags);
	}
}