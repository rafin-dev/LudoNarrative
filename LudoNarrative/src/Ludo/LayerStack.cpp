#include "ldpch.h"
#include "LayerStack.h"

namespace Ludo {

	LayerStack::LayerStack()
	{
		m_LayerInsertIndex = 0;
	}
	
	void LayerStack::Clear()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	// Currently, theres nothing preventing someone from deleting a layer as if it was a overlay and messing up the whole thing
	void LayerStack::PopLayer(Layer* layer)
	{
		auto ite = std::find(m_Layers.begin(), m_Layers.end(), layer);

		if (ite != m_Layers.end())
		{
			m_Layers.erase(ite);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto ite = std::find(m_Layers.begin(), m_Layers.end(), overlay);

		if (ite != m_Layers.end())
		{
			m_Layers.erase(ite);
		}
	}

}