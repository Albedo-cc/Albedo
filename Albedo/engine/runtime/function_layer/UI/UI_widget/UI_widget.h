#pragma once

#include <AlbedoRHI.hpp>

#include <imgui.h>

namespace Albedo {
namespace Runtime
{
	
	class UIWidget
	{
		friend class UISystem; // Created By UISystem

		class Texture
		{
		public:
			void Update(std::shared_ptr<RHI::VMA::Image> image);
			operator ImTextureID() { return *m_descriptor_set; }

		public:
			Texture() = delete;
			Texture(std::shared_ptr<RHI::DescriptorSet> descriptor_set) :
				m_descriptor_set{ std::move(descriptor_set) } {}

		private:
			std::shared_ptr<RHI::DescriptorSet> m_descriptor_set;
		};
	};

}} // namespace Albedo::Runtime