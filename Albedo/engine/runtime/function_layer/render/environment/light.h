#pragma once

#include <AlbedoRHI.hpp>
#include <core/math/math.h>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	class Light
	{
	public:
		struct Parameters
		{
			Vector4f light_position;
			Vector4f view_position;
		};
		Parameters parameters;

		std::shared_ptr<RHI::VMA::Buffer> GetLightData()
		{
			m_light_parameter_buffer->Write(&parameters);
			return m_light_parameter_buffer;
		}

	public:
		Light() = delete;
		Light(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VMA::Buffer> m_light_parameter_buffer;
	};
	
}} // namespace Albedo::Runtime