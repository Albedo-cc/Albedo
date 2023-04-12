#include "scene.h"

#include "../paint_box/palette.h"

namespace Albedo {
namespace Runtime
{

	Scene::Scene(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		m_vulkan_context{ std::move(vulkan_context) }
	{
		
	}

	void Scene::Load(std::shared_ptr<Model> model)
	{
		
	}
	
}} // namespace Albedo::Runtime