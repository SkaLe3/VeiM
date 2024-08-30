#include "LayerStack.h"
#include "VeiM/IO/Log.h"



VeiM::LayerStack::~LayerStack()
{
	Clear();
}

void VeiM::LayerStack::PushLayer(Layer* layer)
{

	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
}

void VeiM::LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
}

void VeiM::LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
	if (it != m_Layers.begin() + m_LayerInsertIndex)
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
	else
	{
		VM_CORE_WARN("Attempt to pop a layer \"{0}\".Layer\"{1}\" do not exist", layer->GetName(), layer->GetName());
	}
}

void VeiM::LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		overlay->OnDetach();
		m_Layers.erase(it);
	}
	else
	{
		VM_CORE_WARN("Attempt to pop a overlay \"{0}\".Overlay\"{1}\" do not exist", overlay->GetName(), overlay->GetName());
	}
}

void VeiM::LayerStack::Clear()
{
	for (Layer* layer : m_Layers)
	{
		layer->OnDetach();
		delete layer;
	}
	m_Layers.clear();
}
