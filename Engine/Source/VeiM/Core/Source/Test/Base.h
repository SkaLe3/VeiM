#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class CORE_API ClassInfo
	{
	public:
		String className; // Should use cached string
		std::vector<String> properties;
		std::vector<String> methods;
	};

	class CORE_API ReflectionSystem
	{
	public:
		static ReflectionSystem& Get()
		{
			static ReflectionSystem instance;
			return instance;
		}

		void RegisterClass(const String& className, const ClassInfo& info)
		{
			m_ClassRegistry[className] = info;
		}

		const ClassInfo* GetClassInfo(const String& className) const
		{
			auto it = m_ClassRegistry.find(className);
			return it != m_ClassRegistry.end() ? &it->second : nullptr;
		}
		const std::unordered_map<String, ClassInfo>& GetClassRegistry()
		{
			return m_ClassRegistry;
		}
	private:
		std::unordered_map<String, ClassInfo> m_ClassRegistry;
	};


	class CORE_API Base
	{
	public:
		virtual ~Base() = default;
		virtual void print() const = 0;
		virtual void Start() = 0;
		virtual void Update(float deltaTime) = 0;

		using FactoryFunction = UniquePtr<Base>();

		static void Registrate(const String& name, FactoryFunction* fp, const ClassInfo& info)
		{
			Registry()[name] = fp;
			ReflectionSystem::Get().RegisterClass(name, info);
		}

		static UniquePtr<Base> Instantiate(const String& name)
		{
			auto it = Registry().find(name);
			return it == Registry().end() ? nullptr : (it->second)();
		}

		template <typename D>
		struct Registrar 
		{
			explicit Registrar(const String& name, const ClassInfo& info)
			{
				Base::Registrate(name, &D::Create, info);
			}
		private:
			Registrar(const Registrar&) = delete;
			Registrar& operator=(const Registrar&) = delete;
		};


	private:
		static std::unordered_map<String, FactoryFunction*>& Registry() {
			static std::unordered_map<String, FactoryFunction*> impl;
			return impl;
		}

	};


}