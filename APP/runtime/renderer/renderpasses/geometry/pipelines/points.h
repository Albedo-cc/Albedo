#pragma once

#include "../renderpass.h"

namespace Albedo{
namespace APP
{

	class PointsPipeline final
		:public GraphicsPipeline
	{
	public:
		virtual void Begin(std::shared_ptr<CommandBuffer> commandbuffer) override;
		virtual void End(std::shared_ptr<CommandBuffer> commandbuffer)	  override;

	public:
		PointsPipeline();
		virtual ~PointsPipeline() noexcept override {};

	protected:
		virtual auto vertex_input_state()	-> const VkPipelineVertexInputStateCreateInfo& override;
		//virtual auto tessellation_state()	-> const VkPipelineTessellationStateCreateInfo&;
		virtual auto input_assembly_state()	-> const VkPipelineInputAssemblyStateCreateInfo& override;
		//virtual auto viewport_state()		-> const VkPipelineViewportStateCreateInfo&;
		//virtual auto rasterization_state()-> const VkPipelineRasterizationStateCreateInfo&;
		//virtual auto multisampling_state()-> const VkPipelineMultisampleStateCreateInfo&;
		virtual auto depth_stencil_state()	-> const VkPipelineDepthStencilStateCreateInfo& override;
		//virtual auto color_blend_state()	-> const VkPipelineColorBlendStateCreateInfo&;
		//virtual auto dynamic_state()		-> const VkPipelineDynamicStateCreateInfo&;
	};
	
}} // namespace Albedo::APP