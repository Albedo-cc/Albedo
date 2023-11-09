#pragma once

#include "../renderpass.h"

namespace Albedo{
namespace APP
{

	class SkyboxPipeline final
		:public GraphicsPipeline
	{
	public:
		virtual void Begin(std::shared_ptr<CommandBuffer> commandbuffer) override;
		virtual void End(std::shared_ptr<CommandBuffer> commandbuffer)	  override;

	public:
		SkyboxPipeline();
		virtual ~SkyboxPipeline() noexcept override {};

	protected:
		virtual auto vertex_input_state()	-> const VkPipelineVertexInputStateCreateInfo& override;
		//virtual auto tessellation_state()	-> VkPipelineTessellationStateCreateInfo;
		//virtual auto input_assembly_state()	-> VkPipelineInputAssemblyStateCreateInfo;
		//virtual auto viewport_state()		-> VkPipelineViewportStateCreateInfo;
		//virtual auto rasterization_state()	-> VkPipelineRasterizationStateCreateInfo;
		//virtual auto multisampling_state()	-> VkPipelineMultisampleStateCreateInfo;
		//virtual auto depth_stencil_state()	-> const VkPipelineDepthStencilStateCreateInfo& override;
		//virtual auto color_blend_state()	-> VkPipelineColorBlendStateCreateInfo;
		//virtual auto dynamic_state()		-> VkPipelineDynamicStateCreateInfo			override;
	};
	
}} // namespace Albedo::APP