#include "widgets.h"

#include "GRI.h"
#include "Internal/RHI.h"

namespace Albedo
{

	template<IsFrameObject T>
	FrameObjectManager<T>::
	FrameObjectManager()
	{
		//sm_registry.emplace_back(shared_from_this<T>());
		m_frameobjects.resize(g_rhi->swapchain.images.size());
		for (auto& obj : m_frameobjects) obj.Create();
	}

	template<IsFrameObject T> inline
	T&
	Albedo::FrameObjectManager<T>::Get()
	{
		return m_frameobjects[g_rhi->swapchain.cursor];
	}


} // namespace Albedo