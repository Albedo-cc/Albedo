#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <stdexcept>

#include <vulkan/vulkan.h>
	
// Predeclartion
typedef struct GLFWwindow			GLFWwindow;
typedef struct VmaAllocator_T*		VmaAllocator;
typedef struct VmaAllocation_T*		VmaAllocation;
typedef struct ktxVulkanDeviceInfo	ktxVulkanDeviceInfo;

namespace Albedo { namespace Graphics
{
	//[GRI OBJECTS]----------------------------------------------------------------------------------------------------------------
	/*Command*/			class CommandPool; class CommandBuffer; class TransientCommandBuffer; class AutoResetCommandBuffer;
	/*Synchronization*/	class Fence; class Semaphore;
	/*Memory*/			class Shader; class Buffer;
	/*Interface Class*/	class RenderPass; class Pipeline; class GraphicsPipeline;
	/*Descriptor*/		class DescriptorSetLayout; class DescriptorPool; class DescriptorSet;
	/*Texture*/			class Texture; class Texture2D; class Cubemap;
	/*Sampler*/			class Sampler;
	//[TIPS]-----------------------------------------------------------------------------------------------------------------------
	// 1. For better Interface Compatibility, you should create GRI objects via [Class]::Create(...).
	//    Importantly, DO NOT create GRI objects without [Class]::Create(...) by yourself!
	//-----------------------------------------------------------------------------------------------------------------------------

	typedef enum QueueFamilyType_
	{
		QueueFamilyType_Graphics,
		QueueFamilyType_Present,
		QueueFamilyType_Transfer,
		QueueFamilyType_Compute,
		MAX_QUEUEFAMILY_TYPE,
	}QueueFamilyType;

	typedef enum ShaderType_
	{
		ShaderType_Vertex	= VK_SHADER_STAGE_VERTEX_BIT,
		ShaderType_Fragment	= VK_SHADER_STAGE_FRAGMENT_BIT,
	}ShaderType;

	typedef enum FenceType_
	{
		FenceType_Unsignaled = 0,
		FenceType_Signaled	 = VK_FENCE_CREATE_SIGNALED_BIT,
	}FenceType;

	typedef enum SemaphoreType_
	{
		SemaphoreType_Unsignaled = 0,
	}SemaphoreType;

	typedef enum CommandBufferLevel_
	{
		CommandBufferLevel_Primary	 = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		CommandBufferLevel_Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
	}CommandBufferLevel;

	typedef enum CommandPoolType_
	{
		CommandPoolType_Normal		= 0, // You should record once for this type.
		CommandPoolType_Transient  = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		CommandPoolType_Resettable = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		//CommandPoolType_Protected  = VK_COMMAND_POOL_CREATE_PROTECTED_BIT,
	}CommandPoolType;

}} // namespace Albedo::Graphics