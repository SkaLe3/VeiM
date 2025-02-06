#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class CORE_API Layer
	{
	public:
		Layer(const String& name = "Layer")
			: m_Name(name){}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnGUI() {}
		
		inline const String& GetName() const { return m_Name; }
	protected:
		String m_Name;
	};
}