#include "UI_widget.h"

#include <backends/imgui_impl_vulkan.h>

namespace Albedo {
namespace Runtime
{
	void UIWidget::Texture::Update(std::shared_ptr<RHI::VMA::Image> image)
	{
		m_descriptor_set->WriteImage(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, image);
	}

}} // namespace Albedo::Runtime