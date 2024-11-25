#pragma once

#include "ldpch.h"

namespace Ludo {

	template<class T, size_t S>
	class GenericSwapChain
	{
	public:
		GenericSwapChain()
		{
		}

		T& GetCurrentItem()
		{
			return m_Itens[m_CurrentItem];
		}

		void Swap()
		{
			m_CurrentItem++;
			m_CurrentItem = m_CurrentItem % m_Itens.size();
		}

		void ForEachElement(const std::function<void(T&)>& action)
		{
			for (int i = 0; i < m_Itens.size(); i++)
			{
				action(m_Itens[i]);
			}
		}

	private:
		std::array<T, S> m_Itens = {};
		uint32_t m_CurrentItem = 0;
	};

}