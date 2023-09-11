#pragma once

#include <vulkan/vulkan.h>

namespace Albedo
{
	typedef enum QueueFamilyType_
	{
		QueueFamilyType_Graphics,
		QueueFamilyType_Present,
		QueueFamilyType_Transfer,
		QueueFamilyType_Compute,
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

} // namespace Albedo