#include "vulkan_manager.h"

namespace Albedo {
namespace RHI
{
	VulkanManager::VulkanManager(GLFWwindow* window) :
		m_context{ window }
	{

	}

	VulkanManager::~VulkanManager()
	{
		
	}

}} // namespace Albedo::RHI