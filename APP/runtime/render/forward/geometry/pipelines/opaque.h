#pragma once

#include <AlbedoGraphics/GRI.h>

namespace Albedo{
namespace APP
{
	
	class OpaquePipeline final
		:public GRI::GraphicsPipeline
	{
	public:
		virtual void Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer) override;
		virtual void End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)	  override;

	public:
		OpaquePipeline();
		virtual ~OpaquePipeline() noexcept override {};

	protected:
		virtual auto vertex_input_state()	-> VkPipelineVertexInputStateCreateInfo		override;
		//virtual auto tessellation_state()	-> VkPipelineTessellationStateCreateInfo;
		//virtual auto input_assembly_state()	-> VkPipelineInputAssemblyStateCreateInfo;
		//virtual auto viewport_state()		-> VkPipelineViewportStateCreateInfo;
		//virtual auto rasterization_state()	-> VkPipelineRasterizationStateCreateInfo;
		//virtual auto multisampling_state()	-> VkPipelineMultisampleStateCreateInfo;
		virtual auto depth_stencil_state()	-> VkPipelineDepthStencilStateCreateInfo;
		//virtual auto color_blend_state()	-> VkPipelineColorBlendStateCreateInfo;
		//virtual auto dynamic_state()		-> VkPipelineDynamicStateCreateInfo			override;
	};
	
}} // namespace Albedo::APP