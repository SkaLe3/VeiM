#pragma once
#include <string>

namespace VeiM
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_Name(name){}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnGUI() {}
		
		inline const std::string& GetName() const { return m_Name; }
	protected:
		std::string m_Name;
	};
}