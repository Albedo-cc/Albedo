#pragma once

#include <imcurio.h>
#include <AlbedoRHI.hpp>

#include <runtime/asset_layer/asset_types.h>

namespace Albedo {
namespace Runtime
{

	class Scene : public ImCurio::ChestItem
	{
		friend class Canvas; // Painted by Canvas
	public:
		void Load(std::shared_ptr<Model> model);

	public:
		Scene() = delete;
		Scene(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
			m_vulkan_context{ std::move(vulkan_context) } {}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		Model::BoundingBox boundingbox;
		std::shared_ptr<RHI::VMA::Buffer> vertices;
		std::shared_ptr<RHI::VMA::Buffer> indices;
		//std::vector<std::shared_ptr<RHI::Sampler>> samplers;	[ Future: now all of images share one default sampler ]
		std::vector<std::shared_ptr<RHI::VMA::Image>>images;
		std::vector<Model::Texture> textures;
		std::vector<std::shared_ptr<RHI::DescriptorSet>> m_descriptor_set_materials;
		std::vector<Model::Material> materials;
		std::vector<std::shared_ptr<Model::Node>> nodes;

	protected:
		virtual void operator()(const ImVec2& size) override;

	protected:
		virtual void on_mouse_is_hovering() override;
		//virtual void on_mouse_left_button_is_clicked() override;
		//virtual void on_mouse_left_button_is_double_clicked() override;
		//virtual void on_mouse_right_button_is_clicked() override;
	};

}} // namespace Albedo::Runtime