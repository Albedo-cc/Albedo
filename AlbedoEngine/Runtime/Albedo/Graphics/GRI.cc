#include "GRI.h"
#include "internal/RHI.h"

#include <Albedo/Core/Log/log.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Core/Norm/types.h>
#include <Albedo/Core/Math/integer.h>
#include <Albedo/Core/Time/stopwatch.h>
#include "../Editor/source/editor.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <ktxvulkan.h>

namespace
{
	using namespace Albedo;
	// Local Objects
	class VMA final
	{
		friend class GRI;
		VmaAllocator handle{ VK_NULL_HANDLE };
		operator VmaAllocator() const { return handle; }
		void Create(); void Destroy();
	};
	static VMA s_vma;

	class KTX final
	{
		friend class GRI;
		ktxVulkanDeviceInfo* context;
		operator ktxVulkanDeviceInfo*() const { return context; }
		void Create(); void Destroy();
	};
	static KTX s_ktx;

	struct RenderTarget
	{
		static inline std::shared_ptr<GRI::Texture> zbuffer; // Shared
		std::shared_ptr<GRI::Texture> image;
		std::shared_ptr<GRI::CommandBuffer> commandbuffer;
		GRI::Fence fence_in_flight{ FenceType_Signaled };
		GRI::Semaphore  semaphore_ready{SemaphoreType_Unsignaled};

		static inline uint32_t FPS{ 0 };
		static inline StopWatch frame_timer;

		static void Initialize();
		static void Destroy();
	};
	static inline std::vector<RenderTarget> s_render_targets;

	// Global Command Pools
	using GRICommandPool = std::unordered_map<QueueFamilyType, std::shared_ptr<GRI::CommandPool>>;
	static std::unordered_map<std::thread::id, GRICommandPool> s_normal_command_pools;
	static std::unordered_map<std::thread::id, GRICommandPool> s_auto_free_command_pools;
	static std::unordered_map<std::thread::id, GRICommandPool> s_auto_reset_command_pools;

	// Global Descriptor Objects
	static std::unordered_map<std::string, std::shared_ptr<GRI::DescriptorSetLayout>>s_descriptor_set_layouts;
	static std::unordered_map<std::thread::id, std::shared_ptr<GRI::DescriptorPool>> s_descriptor_pools;

	// Global Resource
	static std::unordered_map<std::string, std::shared_ptr<GRI::Sampler>> s_global_samplers;
	static std::unordered_map<std::string, std::shared_ptr<GRI::Texture>> s_global_textures;
}

namespace Albedo
{

	void 
	GRI::
	Initialize(const GRICreateInfo& createinfo)
	{
		Log::Debug("Albedo GRI is being initialized...");

		g_rhi->Initialize(RHICreateInfo
		{
			.app_name	  = createinfo.app_name,
			.app_version  = createinfo.app_version,
			.app_window	  = createinfo.app_window,
			.msg_callback = createinfo.msg_callback
		});

		s_vma.Create();
		s_ktx.Create();

		// Init Default Global Resource
		RegisterGlobalSampler("Default", Sampler::Create({}));
		RegisterGlobalSampler("Cubemap", Sampler::Create({ .wrap_mode{.U = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER } }));

		GRI::RegisterGlobalDescriptorSetLayout(
			// [0]CIS
			GRI::MakeID({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}),
			GRI::DescriptorSetLayout::Create({
				VkDescriptorSetLayoutBinding
				{
					.binding = 0,
					.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
					.pImmutableSamplers = nullptr,
				}
			}));

		RenderTarget::Initialize();
	}

	void 
	GRI::
	Terminate() noexcept
	{
		Log::Debug("Albedo GRI is being terminated...");
		vkDeviceWaitIdle(g_rhi->device);
		
		RenderTarget::Destroy();

		s_global_samplers.clear();
		s_global_textures.clear();

		s_descriptor_pools.clear();
		s_descriptor_set_layouts.clear();

		s_vma.Destroy();
		s_ktx.Destroy();

		s_normal_command_pools.clear();
		s_auto_free_command_pools.clear();
		s_auto_reset_command_pools.clear();

		g_rhi->Terminate();
	}

	void
	GRI::
	recreate_swapchain()
	{
		g_rhi->recreate_swapchain();
		RenderTarget::Destroy();
		RenderTarget::Initialize();
		Editor::Recreate();
	}
	
	std::shared_ptr<GRI::CommandPool>
	GRI::
	GetGlobalCommandPool(
		CommandPoolType type,
		QueueFamilyType queue,
		std::thread::id thread_id/* = std::this_thread::get_id()*/)
	{
		switch (type)
		{
		case CommandPoolType_Normal:
		{
			auto& commandPools = s_normal_command_pools[thread_id];
			auto& commandPool = commandPools[queue];
			if (commandPool == nullptr)
			{
				Log::Debug("Albedo GRI is creating a new Normal Command Pool.");
				commandPool = CommandPool::Create(CommandPool::CreateInfo
				{
					.queue_family = queue,
					.type = CommandPoolType_Normal,
				});
			}
			return commandPool;
		}
		case CommandPoolType_Transient:
		{
			auto& commandPools = s_auto_free_command_pools[thread_id];
			auto& commandPool = commandPools[queue];
			if (commandPool == nullptr)
			{
				Log::Debug("Albedo GRI is creating a new Transient Command Pool.");
				commandPool = CommandPool::Create(CommandPool::CreateInfo
				{
					.queue_family = queue,
					.type = CommandPoolType_Transient,
				});
			}
			return commandPool;
		}
		case CommandPoolType_Resettable:
		{
			auto& commandPools = s_auto_reset_command_pools[thread_id];
			auto& commandPool = commandPools[queue];
			if (commandPool == nullptr)
			{
				Log::Debug("Albedo GRI is creating a new Resettable Command Pool.");
				commandPool = CommandPool::Create(CommandPool::CreateInfo
				{
					.queue_family = queue,
					.type = CommandPoolType_Resettable,
				});
			}
			return commandPool;
		}
		default:ALBEDO_ASSERT(false);
		}
		return nullptr;
	}

	void
	GRI::
	RegisterGlobalDescriptorSetLayout(std::string id, std::shared_ptr<DescriptorSetLayout> descriptor_set_layout)
	{
		auto target = s_descriptor_set_layouts.find(id);
		if (target == s_descriptor_set_layouts.end())
		{
			Log::Debug("Albedo GRI is registering a new Descriptor Set Layout({}).", id);
			s_descriptor_set_layouts.emplace(std::move(id), std::move(descriptor_set_layout));
		}
		else Log::Fatal("Failed to register duplicate Vulkan Descriptor Set Layouts({})!", id);
	}

	void
	GRI::
	RegisterGlobalSampler(std::string id, std::shared_ptr<Sampler> sampler)
	{
		auto target = s_global_samplers.find(id);
		if (target == s_global_samplers.end())
		{
			Log::Debug("Albedo GRI is registering a new Sampelr({}).", id);
			s_global_samplers.emplace(std::move(id), std::move(sampler));
		}
		else Log::Fatal("Failed to register duplicate GRI Sampelr({})!", id);
	}

	void
	GRI::
	RegisterGlobalTexture(std::string id, std::shared_ptr<Texture> texture)
	{
		auto target = s_global_textures.find(id);
		if (target == s_global_textures.end())
		{
			Log::Debug("Albedo GRI is registering a new Texture({}).", id);
			s_global_textures.emplace(std::move(id), std::move(texture));
		}
		else Log::Fatal("Failed to register duplicate GRI Texture({})!", id);
	}

	std::string
	GRI::MakeID(const std::vector<VkDescriptorType>& types_in_order)
	{
		ALBEDO_ASSERT(types_in_order.size() < 10 && "Unexpected Size!");
		static std::string alias[]
		{
			"S  ",	//VK_DESCRIPTOR_TYPE_SAMPLER = 0
			"CIS",	//VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1
			"SaI",	//VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2
			"StI",	//VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3
			"UTB",	//VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4
			"STB",	//VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5
			"UB ",	//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6
			"SB ",	//VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7
			"UBD",	//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8
			"SBD",	//VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9
			"IA ",	//VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10
		};

		std::string id;
		id.reserve(types_in_order.size() * (1 + alias[0].size()));
		for (size_t i = 0; i < types_in_order.size(); ++i)
		{
			ALBEDO_ASSERT(types_in_order[i] <= VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
			id += char('0' + i) + alias[types_in_order[i]];
		}
		return id;
	}

	size_t
	GRI::
	PadUniformBufferSize(size_t original_size)
	{
		// Calculate required alignment based on minimum device offset alignment
		// Thanks to Sascha Willems and his Vulkan samples for the snippet.
		// https://github.com/SaschaWillems/Vulkan/tree/master/examples/dynamicuniformbuffer
		size_t minUboAlignment = g_rhi->GPU.properties.limits.minUniformBufferOffsetAlignment;
		size_t alignedSize = original_size;

		if (minUboAlignment > 0)
		{
			alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}
		else ALBEDO_UNEXPECTED_ASSERT;

		return alignedSize;
	}

	std::shared_ptr<GRI::DescriptorPool>
	GRI::
	GetGlobalDescriptorPool(std::thread::id thread_id/* = std::this_thread::get_id()*/)
	{
		auto& target = s_descriptor_pools[thread_id];
		if (target == nullptr)
		{
			Log::Debug("Albedo GRI is creating a new Global Descriptor Pool.");

			constexpr uint32_t LIMIT_SETS = 100;
			target = DescriptorPool::Create(std::vector<VkDescriptorPoolSize>
				{
					{ VK_DESCRIPTOR_TYPE_SAMPLER,					100 },
					{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	100 },
					{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				100 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				100 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		100 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		100 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			100 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			100 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	100 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	100 },
					{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			100 },
				}, LIMIT_SETS);
			return target;
		}
		else return target;
	}

	std::shared_ptr<GRI::DescriptorSetLayout>
	GRI::
	GetGlobalDescriptorSetLayout(const std::string& id)
	{
		auto target = s_descriptor_set_layouts.find(id);
		if (target != s_descriptor_set_layouts.end())
		{
			return target->second;
		}
		else Log::Fatal("Failed to get {} Descriptor Set Layouts!", id);
	}

	std::shared_ptr<GRI::Sampler>
	GRI::
	GetGlobalSampler(const std::string& id)
	{
		auto target = s_global_samplers.find(id.data());
		if (target != s_global_samplers.end())
		{
			return target->second;
		}
		else Log::Fatal("Failed to get {} Sampler!", id);
	}

	std::shared_ptr<GRI::Texture>
	GRI::GetGlobalTexture(const std::string& id)
	{
		auto target = s_global_textures.find(id.data());
		if (target != s_global_textures.end())
		{
			return target->second;
		}
		else Log::Fatal("Failed to get {} Texture!", id);
	}

	VkQueue
	GRI::
	GetQueue(QueueFamilyType queue_family, uint32_t index/* = 0*/)
	{
		switch (queue_family)
		{
		case QueueFamilyType_Graphics:
			ALBEDO_ASSERT(g_rhi->device.queue_families.graphics.queues.size() > index);
			return g_rhi->device.queue_families.graphics.queues[index];
		case QueueFamilyType_Present:
			ALBEDO_ASSERT(g_rhi->device.queue_families.present.queues.size() > index);
			return g_rhi->device.queue_families.present.queues[index];
		case QueueFamilyType_Transfer:
			ALBEDO_ASSERT(g_rhi->device.queue_families.transfer.queues.size() > index);
			return g_rhi->device.queue_families.transfer.queues[index];
		case QueueFamilyType_Compute:
			ALBEDO_ASSERT(g_rhi->device.queue_families.compute.queues.size() > index);
			return g_rhi->device.queue_families.compute.queues[index];
		default: ALBEDO_UNEXPECTED_ASSERT;
		}
	}

	uint32_t
	GRI::
	GetQueueFamilyIndex(QueueFamilyType queue_family)
	{
		switch (queue_family)
		{
		case QueueFamilyType_Graphics:
			ALBEDO_ASSERT(g_rhi->device.queue_families.graphics.index.has_value());
			return g_rhi->device.queue_families.graphics.index.value();
		case QueueFamilyType_Present:
			ALBEDO_ASSERT(g_rhi->device.queue_families.present.index.has_value());
			return g_rhi->device.queue_families.present.index.value();
		case QueueFamilyType_Transfer:
			ALBEDO_ASSERT(g_rhi->device.queue_families.transfer.index.has_value());
			return g_rhi->device.queue_families.transfer.index.value();
		case QueueFamilyType_Compute:
			ALBEDO_ASSERT(g_rhi->device.queue_families.compute.index.has_value());
			return g_rhi->device.queue_families.compute.index.value();
		default: ALBEDO_UNEXPECTED_ASSERT;
		}
	}

	std::shared_ptr<const GRI::Texture>
	GRI::
	GetCurrentRenderTarget()
	{
		return s_render_targets[g_rhi->swapchain.cursor].image;
	}

	size_t
	GRI::
	GetRenderTargetCount()
	{
		return g_rhi->swapchain.images.size();
	}

	uint32_t
	GRI::
	GetRenderTargetCursor()
	{
		return g_rhi->swapchain.cursor;
	}

	VkFormat
	GRI::
	GetRenderTargetFormat()
	{
		return g_rhi->swapchain.format;
	}

	std::shared_ptr<const GRI::Texture>
	GRI::
	GetZBuffer()
	{
		return RenderTarget::zbuffer;
	}

	GRI::Fence::
	Fence(FenceType type)
	{
		VkFenceCreateInfo fenceCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = FenceType_Signaled & type ? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags(0),
		};
		if (vkCreateFence(
			g_rhi->device,
			&fenceCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Fence!");
	}

	GRI::Fence::
	~Fence() noexcept
	{
		vkDestroyFence(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	void 
	GRI::Fence::
	Wait(uint64_t timeout/* = std::numeric_limits<uint64_t>::max()*/)
	{
		vkWaitForFences(g_rhi->device, 1, &m_handle, VK_TRUE, timeout);
	}

	void 
	GRI::Fence::
	Reset()
	{
		vkResetFences(g_rhi->device, 1, &m_handle);
	}

	bool 
	GRI::Fence::
	IsReady()
	{
		return VK_SUCCESS == vkGetFenceStatus(g_rhi->device, m_handle);
	}

	GRI::Semaphore::
	Semaphore(SemaphoreType type)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.flags = VkSemaphoreCreateFlags(type),
		};
		if (vkCreateSemaphore(
			g_rhi->device,
			&semaphoreCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Semaphore!");
	}

	GRI::Semaphore::
	~Semaphore() noexcept
	{
		vkDestroySemaphore(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	GRI::CommandBuffer::
	CommandBuffer(
		std::shared_ptr<CommandPool> command_pool, 
		const CreateInfo& createinfo) :
		m_parent{ std::move(command_pool) },
		m_settings{ createinfo } 
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = *m_parent,
			.level = VkCommandBufferLevel(m_settings.level),
			.commandBufferCount = 1
		};

		if (vkAllocateCommandBuffers(
			g_rhi->device,
			&commandBufferAllocateInfo, 
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Command Buffer!");
	}

	GRI::
	CommandBuffer::
	~CommandBuffer() noexcept
	{
		if (VK_NULL_HANDLE == m_handle) return;

		vkFreeCommandBuffers(g_rhi->device, *m_parent, 1, &m_handle);
		m_handle = VK_NULL_HANDLE;
	}

	void 
	GRI::CommandBuffer::
	Begin()
	{
		ALBEDO_ASSERT(!IsRecording() && "You cannot Begin() a recording Vulkan Command Buffer!");

		VkCommandBufferBeginInfo commandBufferBeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0x0,
			.pInheritanceInfo = nullptr
		};
		if (vkBeginCommandBuffer(m_handle, &commandBufferBeginInfo) != VK_SUCCESS)
			Log::Fatal("Failed to begin the Vulkan Command Buffer!");

		m_is_recording = true;
	}
	void 
	GRI::CommandBuffer::
	End()
	{
		ALBEDO_ASSERT(IsRecording() && "You cannot End() an idle Vulkan Command Buffer!");

		if (vkEndCommandBuffer(m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to end the Vulkan Command Buffer!");

		m_is_recording = false;
	}

	void 
	GRI::CommandBuffer::
	Submit(const SubmitInfo& submitinfo, VkFence signal_fence/* = VK_NULL_HANDLE*/)
	{
		ALBEDO_ASSERT(!IsRecording() && "You should call End() before Submit()!");

		auto submitInfo = VkSubmitInfo
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = static_cast<uint32_t>(submitinfo.wait_semaphores.size()),
			.pWaitSemaphores	= submitinfo.wait_semaphores.data(),
			.pWaitDstStageMask  = &submitinfo.wait_stages,
			.commandBufferCount = 1,
			.pCommandBuffers	= &m_handle,
			.signalSemaphoreCount = static_cast<uint32_t>(submitinfo.signal_semaphores.size()),
			.pSignalSemaphores	= submitinfo.signal_semaphores.data()
		};

		if (vkQueueSubmit(
			GetQueue(m_parent->m_settings.queue_family), 
			1,
			&submitInfo,
			signal_fence) != VK_SUCCESS)
			Log::Fatal("Failed to submit the Vulkan Command Buffer!");
	}

	void 
	GRI::TransientCommandBuffer::
	TransientCommandBuffer::Begin()
	{
		ALBEDO_ASSERT(!IsRecording() && "You cannot Begin() a recording Vulkan Command Buffer!");

		VkCommandBufferBeginInfo commandBufferBeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr
		};
		if (vkBeginCommandBuffer(m_handle, &commandBufferBeginInfo) != VK_SUCCESS)
			Log::Fatal("Failed to begin the Vulkan Command Buffer!");

		m_is_recording = true;
	}

	void 
	GRI::AutoResetCommandBuffer::
	Begin()
	{
		ALBEDO_ASSERT(!IsRecording() && "You cannot Begin() a recording Vulkan Command Buffer!");

		vkResetCommandBuffer(m_handle, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0x0,
			.pInheritanceInfo = nullptr
		};
		if (vkBeginCommandBuffer(m_handle, &commandBufferBeginInfo) != VK_SUCCESS)
			Log::Fatal("Failed to begin the Vulkan Command Buffer!");

		m_is_recording = true;
	}
   
	GRI::CommandPool::
	CommandPool(const CommandPool::CreateInfo& createinfo) :
		m_settings{ createinfo }
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VkCommandPoolCreateFlags(m_settings.type),
			.queueFamilyIndex = GetQueueFamilyIndex(m_settings.queue_family)
		};

		if (vkCreateCommandPool(
			g_rhi->device,
			&commandPoolCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Command Pool!");
	}

	GRI::CommandPool::
	~CommandPool() noexcept
	{
		// Command buffers will be automatically freed when their command pool is destroyed
		vkDestroyCommandPool(g_rhi->device, m_handle,g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	void
	GRI::CommandPool::
	SubmitCommandBuffers(const std::vector<VkSubmitInfo>& submitinfos, VkFence signal_fence/* = VK_NULL_HANDLE*/)
	{
		if (vkQueueSubmit(GetQueue(m_settings.queue_family), 
			submitinfos.size(),
			submitinfos.data(),
			signal_fence) != VK_SUCCESS)
			Log::Fatal("Failed to submit the Vulkan Command Buffers!");
	}

	// Don't need createinfo unless CommandPoolType_Customize
	std::shared_ptr<GRI::CommandBuffer> 
	GRI::CommandPool::
	AllocateCommandBuffer(const CommandBuffer::CreateInfo& createinfo/* = {}*/)
	{
		std::shared_ptr<GRI::CommandBuffer> commandbuffer;

		switch (m_settings.type)
		{
		case CommandPoolType_Normal:
			commandbuffer = std::make_shared<CommandBuffer>(shared_from_this(), createinfo);
			break;
		case CommandPoolType_Resettable:
			commandbuffer = std::make_shared<AutoResetCommandBuffer>(shared_from_this(), createinfo);
			break;
		case CommandPoolType_Transient:
			commandbuffer = std::make_shared<TransientCommandBuffer>(shared_from_this(), createinfo);
			break;
		default:ALBEDO_ASSERT(false);
		}

		return commandbuffer;
	}

	GRI::DescriptorSetLayout::
	DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings):
		m_bindings{std::move(descriptor_bindings)}
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = static_cast<uint32_t>(m_bindings.size()),
			.pBindings = m_bindings.data()
		};

		if (vkCreateDescriptorSetLayout(
			g_rhi->device,
			&descriptorSetLayoutCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Descriptor Set Layout!");
	}

	GRI::DescriptorSetLayout::
	~DescriptorSetLayout() noexcept
	{
		vkDestroyDescriptorSetLayout(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	const VkDescriptorSetLayoutBinding&
	GRI::DescriptorSetLayout::
	GetBinding(uint32_t index) const
	{
		ALBEDO_ASSERT(m_bindings.size() > index);
		return m_bindings[index];
	}

	GRI::DescriptorPool::
	DescriptorPool(const std::vector<VkDescriptorPoolSize>& pool_size, uint32_t limit_max_sets)
	{
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = limit_max_sets,
			.poolSizeCount = static_cast<uint32_t>(pool_size.size()),
			.pPoolSizes = pool_size.data()
		};
		if (vkCreateDescriptorPool(
			g_rhi->device,
			&descriptorPoolCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Descriptor Pool!");
	}

	GRI::DescriptorPool::
	~DescriptorPool() noexcept
	{
		//ALBEDO_ASSERT(weak_from_this().use_count() == 1 && "You should free all descriptor sets created by current descriptor pool first.");
		vkDestroyDescriptorPool(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	std::shared_ptr<GRI::DescriptorSet>
	GRI::DescriptorPool::
	AllocateDescriptorSet(std::shared_ptr<DescriptorSetLayout> layout)
	{
		return std::make_shared<GRI::DescriptorSet>(shared_from_this(), layout);
	}

	void
	GRI::
	UpdateDescriptorSets(const std::vector<VkWriteDescriptorSet>& updateinfo)
	{
		vkUpdateDescriptorSets(g_rhi->device, updateinfo.size(), updateinfo.data(), 0, nullptr);
	}

	VkWriteDescriptorSet
	GRI::DescriptorSet::
	BindToBuffer(uint32_t binding, std::shared_ptr<Buffer> buffer,
				size_t offset, size_t size)
	{
		auto& bindinginfo = m_layout->GetBinding(binding);

		static VkDescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = *buffer;
		descriptorBufferInfo.offset = offset;
		descriptorBufferInfo.range	= size;

		return VkWriteDescriptorSet
		{
			.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			= m_handle,
			.dstBinding		= binding,
			.dstArrayElement= 0,
			.descriptorCount= 1,
			.descriptorType = bindinginfo.descriptorType,
			.pImageInfo		= nullptr,
			.pBufferInfo	= &descriptorBufferInfo,
			.pTexelBufferView = nullptr
		};
	}

	VkWriteDescriptorSet
	GRI::DescriptorSet::
	BindToTexture(uint32_t binding, std::shared_ptr<Texture> texture)
	{
		auto& bindinginfo = m_layout->GetBinding(binding);

		static VkDescriptorImageInfo imageinfo{};
		imageinfo.sampler	  = *texture->GetSampler();
		imageinfo.imageView	  = texture->GetView();
		imageinfo.imageLayout = texture->GetLayout();

		return VkWriteDescriptorSet
		{
			.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			= m_handle,
			.dstBinding		= binding,
			.dstArrayElement= 0,
			.descriptorCount= 1,
			.descriptorType	= bindinginfo.descriptorType,
			.pImageInfo		= &imageinfo,
			.pBufferInfo	= nullptr,
			.pTexelBufferView = nullptr,
		};
	}

	GRI::DescriptorSet::
	DescriptorSet(std::shared_ptr<DescriptorPool> parent, std::shared_ptr<DescriptorSetLayout> layout) :
		m_parent{ std::move(parent) },
		m_layout{ std::move(layout) }
	{
		VkDescriptorSetLayout descriptor_set_layout = *m_layout;
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = *m_parent,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptor_set_layout,
		};

		if (vkAllocateDescriptorSets(
			g_rhi->device,
			&descriptorSetAllocateInfo,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Descriptor Sets!");
	}

	GRI::DescriptorSet::
	~DescriptorSet() noexcept
	{
		if (VK_NULL_HANDLE == m_handle) return;

		vkFreeDescriptorSets(g_rhi->device, *m_parent, 1, &m_handle);
		m_handle = VK_NULL_HANDLE;
	}

	GRI::Shader::
	Shader(ShaderType type, std::vector<char> code) :
		m_type{ type },
		m_code{ std::move(code) }
	{
		// Register Shader Module
		VkShaderModuleCreateInfo shaderModuleCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = m_code.size(),
			.pCode = reinterpret_cast<const uint32_t*>(m_code.data())
		};

		if (vkCreateShaderModule(
			g_rhi->device,
			&shaderModuleCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Shader!");
	}

	GRI::Shader::
	~Shader() noexcept
	{
		vkDestroyShaderModule(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	GRI::Buffer::
	Buffer(const CreateInfo& createinfo)
	{
		VkBufferCreateInfo bufferCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = createinfo.size,
			.usage = createinfo.usage,
			.sharingMode = createinfo.mode
		};

		VmaAllocationCreateFlags vmaFlags = 0;
		if (Property::Readable & createinfo.properties)	  vmaFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		if (Property::Writable & createinfo.properties)	  vmaFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		if (Property::Persistent & createinfo.properties) vmaFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		
		VmaAllocationCreateInfo allocationInfo
		{
			.flags = vmaFlags,
			.usage = VMA_MEMORY_USAGE_AUTO,
		}; 

		if (vmaCreateBuffer(
			s_vma,
			&bufferCreateInfo,
			&allocationInfo,
			&m_handle,
			&m_allocation,
			nullptr) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Buffer!");
	}

	GRI::Buffer::
	~Buffer() noexcept
	{
		vmaDestroyBuffer(s_vma, m_handle, m_allocation);
		m_handle = VK_NULL_HANDLE;
	}

	void
	GRI::Buffer::
	WriteAll(void* data)
	{
		Write(data, GetSize(), 0); // Write All
	}

	void
	GRI::Buffer::
	Write(void* data, size_t size, size_t offset)
	{
		ALBEDO_ASSERT(m_allocation->IsMappingAllowed() && "This buffer is not mapping-allowed!");
		if (offset + size > GetSize())
		{
			Log::Error("Failed to write {} byte(s) with {} bytes offset to buffer (size:{}).", size, offset, GetSize());
		}
		
		Byte* mappedArea;
		if (m_allocation->IsPersistentMap())
		{
			mappedArea = reinterpret_cast<Byte*>(m_allocation->GetMappedData());
			memcpy(mappedArea + offset, data, size);
		}
		else
		{
			vmaMapMemory(s_vma, m_allocation, reinterpret_cast<void**>(&mappedArea));
			memcpy(mappedArea + offset, data, size);
			vmaUnmapMemory(s_vma, m_allocation);
		}
	}

	void* 
	GRI::Buffer::
	Access()
	{
		ALBEDO_ASSERT(m_allocation->IsPersistentMap() && "This buffer is not persistently mapped!");
		return m_allocation->GetMappedData();
	}

	void 
	GRI::Buffer::
	CopyTo(std::shared_ptr<CommandBuffer> commandbuffer,
		   std::shared_ptr<Buffer> target,
		   const CopyInfo& copyinfo/* = {}*/) const
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording() && "You have to ensure that the command buffer is recording while using XXXCommand funcitons!");
		VkDeviceSize range = copyinfo.range ? copyinfo.range : GetSize();
		ALBEDO_ASSERT(range <= (target->GetSize() - copyinfo.target_offset) && "You cannot copy data to another small buffer!");

		VkBufferCopy bufferCopy
		{
			.srcOffset = copyinfo.source_offset,
			.dstOffset = copyinfo.target_offset,
			.size = range,
		};

		vkCmdCopyBuffer(*commandbuffer, m_handle, *target, 1, &bufferCopy);
	}

	void 
	GRI::Buffer::
	CopyFrom(std::shared_ptr<CommandBuffer> commandbuffer,
			 std::shared_ptr<Buffer> target,
			 const CopyInfo& copyinfo/* = {}*/)
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording() && "You have to ensure that the command buffer is recording while using XXXCommand funcitons!");
		VkDeviceSize range = copyinfo.range ? copyinfo.range : target->GetSize();
		ALBEDO_ASSERT(range <= (GetSize() - copyinfo.source_offset) && "You cannot copy data from another big buffer!");

		VkBufferCopy bufferCopy
		{
			.srcOffset = copyinfo.source_offset,
			.dstOffset = copyinfo.target_offset,
			.size = range,
		};

		vkCmdCopyBuffer(*commandbuffer, *target, m_handle, 1, &bufferCopy);
	}

	
	VkDeviceSize 
	GRI::Buffer::
	GetSize() const
	{
		return m_allocation->GetSize();
	}

	GRI::Texture::
	Texture(CreateInfo createinfo, std::shared_ptr<Sampler> sampler/* = nullptr*/) :
		m_settings{ std::move(createinfo) },
		m_sampler{ std::move(sampler) }
	{
		// Judge if valid extent
		if (!m_settings.extent.width || !m_settings.extent.height || !m_settings.extent.depth)
		{
			Log::Fatal("Failed to create Image with extent ({}, {}, {}).",
				m_settings.extent.width, m_settings.extent.height, m_settings.extent.depth);
		}

		// Judge if both width and height is the power of 2.
		if (!IsPowerOfTwo(m_settings.extent.width) || !IsPowerOfTwo(m_settings.extent.height))
		{
			Log::Warn("Bad Texture Extent({}, {}), it is better to create a texture with a power of two size.",
				m_settings.extent.width, m_settings.extent.height);
		}

		VkImageType image_type = 
			m_settings.extent.depth > 1 ? 
			VK_IMAGE_TYPE_3D : 
			VkImageType(VK_IMAGE_TYPE_1D + (m_settings.extent.height > 1));

		VkImageCreateInfo imageCreateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags			= 0x0,
			.imageType		= image_type,
			.format			= m_settings.format,
			.extent			= m_settings.extent,
			.mipLevels		= m_settings.mipLevels,
			.arrayLayers	= m_settings.arrayLayers,
			.samples		= m_settings.samples,
			.tiling			= m_settings.tiling, // P206
			.usage			= m_settings.usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.sharingMode	= VK_SHARING_MODE_EXCLUSIVE, // The image will only be used by one queue family: the one that supports graphics (and therefore also) transfer operations.
			.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED, // P207 Top
		};

		VmaAllocationCreateInfo allocationInfo
		{
			.flags = 0x0,
			.usage = VMA_MEMORY_USAGE_AUTO,
			.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};

		if (vmaCreateImage(
			s_vma,
			&imageCreateInfo,
			&allocationInfo,
			&m_handle,
			&m_allocation,
			nullptr) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Image!");

		m_settings.viewType = m_settings.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM?
							  VkImageViewType(image_type) : m_settings.viewType;
		VkImageViewCreateInfo imageViewCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image		= m_handle,
			.viewType	= m_settings.viewType,
			.format		= m_settings.format,
			.components = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange
			{
				.aspectMask		= m_settings.aspect,
				.baseMipLevel	= 0,
				.levelCount		= m_settings.mipLevels,
				.baseArrayLayer = 0,
				.layerCount		= m_settings.arrayLayers,
			}
		};
		if (vkCreateImageView(
			g_rhi->device,
			&imageViewCreateInfo,
			g_rhi->allocator,
			&m_view
			) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Image View!");

		if (!m_sampler)
		{
			Log::Debug("Using the default sampler to initialize a texture.");
			m_sampler = GRI::GetGlobalSampler("Default");
		}
	}

	GRI::Texture::
	Texture(std::string_view ktx_texture_path,
			std::shared_ptr<Sampler> sampler/* = nullptr*/)
	{
		ALBEDO_WORK_IN_PROGRESS("Wait for KTX 4.2.2 to support VMA subAllocator.");

		//if (ktxTexture_CreateFromNamedFile( // [TODO]: Move to CPU end.
		//	ktx_texture_path.data(),
		//	KTX_TEXTURE_CREATE_NO_FLAGS,
		//	&m_info) != KTX_SUCCESS)
		//	Log::Fatal("Failed to create KTX Texture from {}.", ktx_texture_path);	

		//ktxVulkanTexture vkTexture{};

		//if (ktxTexture_VkUploadEx(
		//	m_info,
		//	sm_ktx,
		//	&vkTexture,
		//	VK_IMAGE_TILING_OPTIMAL,
		//	VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		//	!= KTX_SUCCESS)
		//	Log::Fatal("Failed to create the KTX Vulkan Texture!");
		//vkTexture;

		//VkImageType image_type = m_info->baseDepth > 1 ? 
		//	VK_IMAGE_TYPE_3D : VkImageType(VK_IMAGE_TYPE_1D + (m_info->baseHeight > 1));

		//VkImageCreateInfo imageCreateInfo
		//{
		//	.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		//	.flags			= 0x0,
		//	.imageType		= image_type,
		//	.format			= VK_FORMAT_B8G8R8A8_SRGB, //[TODO]: use macro or config
		//	.extent			= {m_info->baseWidth, m_info->baseHeight, m_info->baseDepth},
		//	.mipLevels		= 1,
		//	.arrayLayers	= 1,
		//	.samples		= VK_SAMPLE_COUNT_1_BIT,
		//	.tiling			= VK_IMAGE_TILING_OPTIMAL, // P206
		//	.usage			= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		//	.sharingMode	= VK_SHARING_MODE_EXCLUSIVE, // The image will only be used by one queue family: the one that supports graphics (and therefore also) transfer operations.
		//	.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED, // P207 Top
		//};

		//VmaAllocationCreateInfo allocationInfo
		//{
		//	.flags = 0x0,
		//	.usage = VMA_MEMORY_USAGE_AUTO,
		//	.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		//};

		//if (vmaCreateImage(
		//	sm_vma,
		//	&imageCreateInfo,
		//	&allocationInfo,
		//	&m_handle,
		//	&m_allocation,
		//	nullptr) != VK_SUCCESS)
		//	Log::Fatal("Failed to create the Vulkan Image!");

	}

	GRI::Texture::
	~Texture() noexcept
	{
		vmaDestroyImage(s_vma, m_handle, m_allocation);
		m_handle = VK_NULL_HANDLE;
		vkDestroyImageView(g_rhi->device, m_view, g_rhi->allocator);
		m_view	 = VK_NULL_HANDLE;
	}

	void
	GRI::Texture::
	fill_convert_layout_info(
		VkImageMemoryBarrier& barrier,
		VkImageLayout source_layout,
		VkImageLayout target_layout,
		VkPipelineStageFlags& from_stage,
		VkPipelineStageFlags& to_stage) const
	{
		// Barriers are primarily used for synchronization purposes, so you must specify which types of operations that involve 
		//the resource must happen before the barrier, 
		//and which operations that involve the resource must wait on the barrier.
		if (VK_IMAGE_LAYOUT_UNDEFINED == source_layout)
		{
			switch (target_layout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				to_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
								VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				to_stage	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				from_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				to_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;

			case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT |
										VK_ACCESS_SHADER_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				to_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
			default: Log::Fatal("Failed to transition the Vulkan Image Layout - Unsupported layout transition!");
			}
		}
		else if (VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL == source_layout)
		{
			switch (target_layout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT |
										VK_ACCESS_SHADER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				from_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				to_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT |
										VK_ACCESS_SHADER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				to_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
			default: Log::Fatal("Failed to transition the Vulkan Image Layout - Unsupported layout transition!");
			}
		}
		else if (VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL == source_layout)
		{
			switch (target_layout)
			{
			case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT |
										VK_ACCESS_SHADER_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				to_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
			default: Log::Fatal("Failed to transition the Vulkan Image Layout - Unsupported layout transition!");
			}
		}
		else if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == source_layout)
		{
			switch (target_layout)
			{
			case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT |
										VK_ACCESS_SHADER_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				to_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				from_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				to_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
			default: Log::Fatal("Failed to transition the Vulkan Image Layout - Unsupported layout transition!");
			}
		}
		else if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == source_layout)
		{
			switch (target_layout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				from_stage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				to_stage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
			default: Log::Fatal("Failed to transition the Vulkan Image Layout - Unsupported layout transition!");
			}
		}
		else Log::Fatal("Failed to transition the Vulkan Image Layout - Unsupported layout transition!");

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = source_layout;
		barrier.newLayout = target_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_handle;
		barrier.subresourceRange = GetSubresourceRange();
	}

	void
	GRI::Texture::
	Resize(std::shared_ptr<CommandBuffer> commandbuffer, const VkExtent3D& extent)
	{
		if (!extent.width || !extent.height || !extent.depth)
			Log::Fatal("Failed to resize Image with extent ({}, {}, {}).",
			extent.width, extent.height, extent.depth);
		
		m_settings.extent = extent;

		vmaDestroyImage(s_vma, m_handle, m_allocation);
		m_handle = VK_NULL_HANDLE;
		vkDestroyImageView(g_rhi->device, m_view, g_rhi->allocator);
		m_view	 = VK_NULL_HANDLE;

		VkImageType image_type = m_settings.extent.depth > 1 ? VK_IMAGE_TYPE_3D : VkImageType(VK_IMAGE_TYPE_1D + (m_settings.extent.height > 1));
		VkImageCreateInfo imageCreateInfo
		{
			.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags			= 0x0,
			.imageType		= image_type,
			.format			= m_settings.format,
			.extent			= m_settings.extent,
			.mipLevels		= m_settings.mipLevels,
			.arrayLayers	= m_settings.arrayLayers,
			.samples		= m_settings.samples,
			.tiling			= m_settings.tiling, // P206
			.usage			= m_settings.usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.sharingMode	= VK_SHARING_MODE_EXCLUSIVE, // The image will only be used by one queue family: the one that supports graphics (and therefore also) transfer operations.
			.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED, // P207 Top
		};

		VmaAllocationCreateInfo allocationInfo
		{
			.flags = 0x0,
			.usage = VMA_MEMORY_USAGE_AUTO,
			.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};

		if (vmaCreateImage(
			s_vma,
			&imageCreateInfo,
			&allocationInfo,
			&m_handle,
			&m_allocation,
			nullptr) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Image!");

		VkImageViewCreateInfo imageViewCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image		= m_handle,
			.viewType	= VkImageViewType(image_type),
			.format		= m_settings.format,
			.components = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange
			{
				.aspectMask		= m_settings.aspect,
				.baseMipLevel	= 0,
				.levelCount		= m_settings.mipLevels,
				.baseArrayLayer = 0,
				.layerCount		= m_settings.arrayLayers,
			}
		};
		if (vkCreateImageView(
			g_rhi->device,
			&imageViewCreateInfo,
			g_rhi->allocator,
			&m_view
			) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Image View!");

		ConvertLayout(commandbuffer, m_layout);
	}

	void
	GRI::Texture::
	Write(std::shared_ptr<GRI::CommandBuffer> commandbuffer, std::shared_ptr<Buffer> data)
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording());
		ALBEDO_ASSERT(data->GetSize() <= GetSize() && "It is not recommanded to write the image from a bigger buffer!");
		
		// Copy buffer to image requires the image to be in the right layout first
		VkBufferImageCopy copyRegion
		{
			.bufferOffset		= 0,
			.bufferRowLength	= 0,
			.bufferImageHeight	= 0,
			.imageSubresource	= GetSubresourceLayers(),
			.imageOffset = {0,0,0},
			.imageExtent = m_settings.extent,
		};

		auto oldLayout = m_layout;
		ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkCmdCopyBufferToImage(*commandbuffer, *data, m_handle, m_layout, 1, &copyRegion);
		ConvertLayout(commandbuffer, oldLayout);
	}

	void
	GRI::Texture::
	Blit(std::shared_ptr<GRI::CommandBuffer> commandbuffer,
		std::shared_ptr<Texture>  target) const
	{
		// Only Blit Mipmap LV.0 by default.
		VkImageBlit blitRegion
		{
			.srcSubresource = this->GetSubresourceLayers(),
			.srcOffsets = 
			{
				{0,0,0},
				{int32_t(this->m_settings.extent.width),
				 int32_t(this->m_settings.extent.height),
				 int32_t(this->m_settings.extent.depth)}
			},
			.dstSubresource = target->GetSubresourceLayers(),
			.dstOffsets =
			{
				{0,0,0},
				{int32_t(target->m_settings.extent.width),
				 int32_t(target->m_settings.extent.height),
				 int32_t(target->m_settings.extent.depth)}
			},
		};
		// Convert Layouts to TransferSRC/DST
		auto targetOldLayout = target->GetLayout();
		VkImageMemoryBarrier barrier_to_transfer{};
		VkImageMemoryBarrier barrier_to_origin{};
		VkPipelineStageFlags from_stage{}, to_stage{};

		if (VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL != m_layout)
		{
			fill_convert_layout_info(barrier_to_transfer,
				m_layout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				from_stage, to_stage);
			vkCmdPipelineBarrier(*commandbuffer, from_stage, to_stage,
				0x0,		// Dependency Flags
				0, nullptr,	// Memory Barrier
				0, nullptr,	// Buffer Memory Barrier
				1, &barrier_to_transfer);
		}
		
		target->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Blit Image
		vkCmdBlitImage(*commandbuffer,
			m_handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			*target,  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blitRegion,
			VK_FILTER_LINEAR);

		// Convert Layouts to Origin
		if (VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL != m_layout)
		{
			fill_convert_layout_info(barrier_to_origin, 
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_layout,
				from_stage, to_stage);
			vkCmdPipelineBarrier(*commandbuffer, from_stage, to_stage,
				0x0,		// Dependency Flags
				0, nullptr,	// Memory Barrier
				0, nullptr,	// Buffer Memory Barrier
				1, &barrier_to_origin);
		}
		target->ConvertLayout(commandbuffer, targetOldLayout);
	}

	void
	GRI::Texture::
	ConvertLayout(std::shared_ptr<GRI::CommandBuffer> commandbuffer, VkImageLayout target_layout)
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording());
		if (target_layout == m_layout) return;

		VkImageMemoryBarrier barrier{};
		VkPipelineStageFlags from_stage{}, to_stage{};
		fill_convert_layout_info(barrier, m_layout, target_layout, from_stage, to_stage);

		vkCmdPipelineBarrier(
			*commandbuffer,
			from_stage,
			to_stage,
			0x0,		// Dependency Flags
			0, nullptr,	// Memory Barrier
			0, nullptr,	// Buffer Memory Barrier
			1, &barrier);

		m_layout = target_layout;
	}

	VkImageSubresourceRange
	GRI::Texture::
	GetSubresourceRange() const
	{
		return VkImageSubresourceRange
		{
			.aspectMask = m_settings.aspect,
			.baseMipLevel = 0,
			.levelCount = m_settings.mipLevels,
			.baseArrayLayer = 0,
			.layerCount = m_settings.arrayLayers,
		};
	}

	VkImageSubresourceLayers
	GRI::Texture::
	GetSubresourceLayers() const
	{
		return VkImageSubresourceLayers
		{
			.aspectMask = m_settings.aspect,
			.mipLevel	= 0,
			.baseArrayLayer = 0,
			.layerCount = m_settings.arrayLayers,
		};
	}

	VkDeviceSize
	GRI::Texture::
	GetSize() const
	{
		return m_allocation->GetSize();
	}

	bool
	GRI::Texture::
	HasStencil() const
	{
		return (VK_FORMAT_S8_UINT <= m_settings.format &&
									 m_settings.format <= VK_FORMAT_D32_SFLOAT_S8_UINT);
	}

	GRI::Sampler::
	Sampler(CreateInfo createinfo):
		m_setting{ std::move(createinfo) }
	{
		VkSamplerCreateInfo samplerCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,

			.magFilter = m_setting.filter.magnify,
			.minFilter = m_setting.filter.minimize,

			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,

			.addressModeU = m_setting.wrap_mode.U,
			.addressModeV = m_setting.wrap_mode.V? m_setting.wrap_mode.V : m_setting.wrap_mode.U,
			.addressModeW = m_setting.wrap_mode.W? m_setting.wrap_mode.W : m_setting.wrap_mode.U,

			.mipLodBias = 0.0,

			.anisotropyEnable = m_setting.enable_anisotropy,
			.maxAnisotropy	  = g_rhi->GPU.properties.limits.maxSamplerAnisotropy,

			.compareEnable	= m_setting.compare_mode? VK_TRUE : VK_FALSE,
			.compareOp		= m_setting.compare_mode,	

			.minLod = 0.0,
			.maxLod = 0.0,

			.borderColor = m_setting.border_color,
			.unnormalizedCoordinates = VK_FALSE
		};

		if (vkCreateSampler(
			g_rhi->device,
			&samplerCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Sampler!");
	}

	GRI::Sampler::
	~Sampler() noexcept
	{
		vkDestroySampler(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}
	
	GRI::Texture2D::
	Texture2D(GRI::Texture::CreateInfo createinfo,
			  std::shared_ptr<Sampler> sampler/* = nullptr*/)
		:Texture{createinfo, sampler? sampler : GRI::GetGlobalSampler("Default")}
	{
		ALBEDO_ASSERT(m_settings.viewType == VK_IMAGE_VIEW_TYPE_2D);
		ALBEDO_ASSERT(m_settings.extent.depth == 1);
	}

	GRI::Cubemap::
	~Cubemap() noexcept
	{
		
	}

	void
	GRI::Texture2D::
	Resize(std::shared_ptr<CommandBuffer> commandbuffer, const VkExtent2D& extent)
	{
		Texture::Resize(commandbuffer, { extent.width, extent.height, 1 });
	}

	GRI::RenderPass::
	RenderPass(std::string name, uint32_t priority) :
		m_name{ std::move(name) },
		m_priority{ priority },
		m_render_area{ {0,0},{g_rhi->swapchain.extent} }
	{
		m_framebuffers.resize(g_rhi->swapchain.images.size());
		sm_renderpass_count++;
	}

	GRI::RenderPass::
	~RenderPass() noexcept
	{
		// Destroy Graphics Pipelines
		for (auto& subpass : m_subpasses)
		{
			subpass.pipeline.reset();
		}
		// Destroy Frame Buffers
		for (auto& frame_buffer : m_framebuffers)
		{
			vkDestroyFramebuffer(g_rhi->device, frame_buffer, g_rhi->allocator);
		}
		vkDestroyRenderPass(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;

		sm_renderpass_count--;
	}

	void
	GRI::RenderPass::
	BEGIN_BUILD(uint32_t flags/* = 0*/)
	{
		add_attachment(AttachmentSetting{ // ST_Color
			.description
			{
				.format			= GRI::GetRenderTargetFormat(),
				.samples		= VK_SAMPLE_COUNT_1_BIT,
				.loadOp			= ClearSTColor & flags? 
								  VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
				.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.finalLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		}});
		for (size_t i = 0; i < m_framebuffers.size(); ++i)
		{
			m_framebuffers[i].render_targets.emplace_back(s_render_targets[i].image->GetView());
		}

		if (ZWrite & flags)
		{
			add_attachment(AttachmentSetting{ // ST_ZBuffer
			.description
			{
				.format			= GRI::GetZBuffer()->GetFormat(),
				.samples		= VK_SAMPLE_COUNT_1_BIT,
				.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				.finalLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			}});
			for (size_t i = 0; i < m_framebuffers.size(); ++i)
			{
				m_framebuffers[i].render_targets.emplace_back(RenderTarget::zbuffer->GetView());
			}
		}
	}

	void
	GRI::RenderPass::
	END_BUILD()
	{
		ALBEDO_ASSERT(VK_SUBPASS_EXTERNAL == uint32_t(-1));
		std::vector<VkSubpassDescription> subpass_descriptions(m_subpasses.size());
		std::vector<VkSubpassDependency>  subpass_dependencies(m_subpasses.size());
		for (uint32_t i = 0; i < subpass_dependencies.size(); ++i)
		{
			// Subpass Descriptions
			subpass_descriptions[i].pipelineBindPoint	= VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass_descriptions[i].inputAttachmentCount= m_subpasses[i].input_attachments.size();
			subpass_descriptions[i].pInputAttachments	= m_subpasses[i].input_attachments.data();
			subpass_descriptions[i].colorAttachmentCount= m_subpasses[i].color_attachments.size();
			subpass_descriptions[i].pColorAttachments	= m_subpasses[i].color_attachments.data();
			subpass_descriptions[i].pDepthStencilAttachment = m_subpasses[i].depth_stencil_attachment.has_value()?
				&m_subpasses[i].depth_stencil_attachment.value() : VK_NULL_HANDLE;
			subpass_descriptions[i].flags = 0x0;
			// Subpass Dependencies
			subpass_dependencies[i].srcSubpass = i - 1;
			subpass_dependencies[i].dstSubpass = i;
			subpass_dependencies[i].srcStageMask  = m_subpasses[i].source_stage_mask;
			subpass_dependencies[i].srcAccessMask = m_subpasses[i].source_access_mask;
			subpass_dependencies[i].dstStageMask  = m_subpasses[i].destination_stage_mask;
			subpass_dependencies[i].dstAccessMask = m_subpasses[i].destination_access_mask;
			subpass_dependencies[i].dependencyFlags = 0x0;
		}

		VkRenderPassCreateInfo renderPassCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = static_cast<uint32_t>(m_attachments.descriptions.size()),
			.pAttachments	 = m_attachments.descriptions.data(),
			.subpassCount	 = static_cast<uint32_t>(subpass_descriptions.size()),
			.pSubpasses		 = subpass_descriptions.data(),
			.dependencyCount = static_cast<uint32_t>(subpass_dependencies.size()),
			.pDependencies	 = subpass_dependencies.data()
		};

		if (vkCreateRenderPass(
			g_rhi->device,
			&renderPassCreateInfo,
			g_rhi->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Render Pass!");

		for (size_t i = 0; i < m_subpasses.size(); ++i)
		{
			auto& pipeline = m_subpasses[i].pipeline;
			assert (pipeline->m_handle == VK_NULL_HANDLE);

			std::vector<VkPipelineShaderStageCreateInfo> shaders(2);
			shaders[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaders[0].stage =  pipeline->m_shader_module.vertex_shader->GetStage();
			shaders[0].module = *pipeline->m_shader_module.vertex_shader;
			shaders[0].pName  = "main";
		
			shaders[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaders[1].stage =  pipeline->m_shader_module.fragment_shader->GetStage();
			shaders[1].module = *pipeline->m_shader_module.fragment_shader;
			shaders[1].pName  = "main";

			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0x0,
				// Shader
				.stageCount = static_cast<uint32_t>(shaders.size()),
				.pStages	= shaders.data(),
				// Pipeline Stages
				.pVertexInputState	= &pipeline->vertex_input_state(),
				.pInputAssemblyState= &pipeline->input_assembly_state(),
				.pTessellationState = &pipeline->tessellation_state(),
				.pViewportState		= &pipeline->viewport_state(),
				.pRasterizationState= &pipeline->rasterization_state(),
				.pMultisampleState	= &pipeline->multisampling_state(),
				.pDepthStencilState = &pipeline->depth_stencil_state(),
				.pColorBlendState	= &pipeline->color_blend_state(),
				.pDynamicState		= &pipeline->dynamic_state(),
				// Pipeline Info
				.layout				= pipeline->m_pipeline_layout,
				.renderPass			= m_handle,
				.subpass			= static_cast<uint32_t>(i),
				.basePipelineHandle = VK_NULL_HANDLE,
				.basePipelineIndex	= 0,
			};

			if (vkCreateGraphicsPipelines(
				g_rhi->device,
				g_rhi->pipeline_cache,
				1,
				&graphicsPipelineCreateInfo,
				g_rhi->allocator,
				&pipeline->m_handle) != VK_SUCCESS)
				Log::Fatal("Failed to create the Vulkan Graphics Pipeline!");
		}

		for (size_t i = 0; i < m_framebuffers.size(); ++i)
		{
			VkFramebufferCreateInfo framebufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = m_handle,
				.attachmentCount = static_cast<uint32_t>(m_framebuffers[i].render_targets.size()),
				.pAttachments = m_framebuffers[i].render_targets.data(),
				.width  = m_render_area.extent.width,
				.height = m_render_area.extent.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(
				g_rhi->device,
				&framebufferCreateInfo,
				g_rhi->allocator,
				&m_framebuffers[i].handle) != VK_SUCCESS)
				Log::Fatal("Failed to create the Vulkan Framebuffer!");
		}
	}

	uint32_t
	GRI::RenderPass::
	add_attachment(AttachmentSetting setting)
	{
		size_t index = m_attachments.descriptions.size();
		m_attachments.descriptions.emplace_back(std::move(setting.description));
		m_attachments.clear_colors.emplace_back(std::move(setting.clearColor));
		return index;
	}

	uint32_t
	GRI::RenderPass::
	add_subpass(SubpassSetting setting)
	{
		ALBEDO_ASSERT(!setting.name.empty());

		size_t index = m_subpasses.size();
		m_subpasses.emplace_back(std::move(setting));
		return index;
	}

	VkAttachmentReference
	GRI::RenderPass::
	get_system_target_reference(SystemTarget target)
	{
		ALBEDO_ASSERT(target < MAX_SYSTEM_TARGET);
		static const VkAttachmentReference references[MAX_SYSTEM_TARGET]
		{
			{ST_Color,		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
			{ST_ZBuffer,	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
		};
		return references[target];
	}

	GRI::RenderPass::SubpassIterator
	GRI::RenderPass::
	Begin(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		ALBEDO_ASSERT(VK_NULL_HANDLE != m_handle && "Please call build() in constructor!");
		// Begin Info
		auto& current_framebuffer = m_framebuffers[g_rhi->swapchain.cursor];
		VkRenderPassBeginInfo renderPassBeginInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass		= m_handle,
			.framebuffer	= current_framebuffer,
			.renderArea		= m_render_area,
			.clearValueCount= static_cast<uint32_t>(m_attachments.clear_colors.size()),
			.pClearValues	= m_attachments.clear_colors.data()
		};

		// Subpass Contents
		VkSubpassContents contents = commandbuffer->GetSubpassContents();

		// Begin Render Pass
		vkCmdBeginRenderPass(*commandbuffer, &renderPassBeginInfo, contents);

		return { commandbuffer, m_subpasses.begin(), m_subpasses.end() };
	}

	void
	GRI::RenderPass::
	End(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording() && "You must Begin() the command buffer before End() the render pass!");
		vkCmdEndRenderPass(*commandbuffer);
	}

	uint32_t
	GRI::RenderPass::
	SeachSubpass(std::string_view name) const
	throw(std::runtime_error)
	{
		for (uint32_t i = 0;i < m_subpasses.size(); ++i)
		{
			if (m_subpasses[i].name == name) return i;
		}
		Log::Error("Failed to find subpass({}) in renderpass({})!", name, m_name);
	}

	void
	GRI::Pipeline::
	Begin(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording() && "You cannot Begin() before beginning the command buffer!");
		vkCmdBindPipeline(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_handle);
	}

	void
	GRI::Pipeline::
	End(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		ALBEDO_ASSERT(commandbuffer->IsRecording() && "You cannot End() before beginning the command buffer!");
	}

	GRI::Pipeline::
	~Pipeline() noexcept
	{
		vkDestroyPipelineLayout(g_rhi->device, m_pipeline_layout, g_rhi->allocator);
		m_pipeline_layout = VK_NULL_HANDLE;
		vkDestroyPipeline(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	GRI::GraphicsPipeline::
	GraphicsPipeline(ShaderModule shader_module):
		m_viewport // Default Viewport with Y-Inversion (https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/#:~:text=Different%20coordinate%20systems%20The%20cause%20for%20this%20is,left%20of%20the%20screen%2C%20with%20Y%20pointing%20downwards.)
		{
			// Lower-left corner of screen. (Unity)
			.x		= 0.0f, 
			.y		= static_cast<float>(g_rhi->swapchain.extent.height),
			.width	= static_cast<float>(g_rhi->swapchain.extent.width),
			.height	= - static_cast<float>(g_rhi->swapchain.extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		},
		m_scissor // Default Scissor
		{
			.offset = { 0, 0 },
			.extent = g_rhi->swapchain.extent,
		},
		m_color_blend // Default Color Blend
		{
			.blendEnable			= VK_TRUE,
			.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp			= VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp			= VK_BLEND_OP_ADD,
			.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT |
									  VK_COLOR_COMPONENT_G_BIT | 
									  VK_COLOR_COMPONENT_B_BIT | 
									  VK_COLOR_COMPONENT_A_BIT,
		},
		m_shader_module{std::move(shader_module)}
	{
		ALBEDO_ASSERT(ShaderType_Vertex   == m_shader_module.vertex_shader->GetType());
		ALBEDO_ASSERT(ShaderType_Fragment == m_shader_module.fragment_shader->GetType());

		// Create Pipeline Layout
		std::vector<VkPushConstantRange> push_constants;
		// [TODO]: Add to Shader or Pipeline directly
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(m_shader_module.descriptor_set_layouts.size()),
			.pSetLayouts = m_shader_module.descriptor_set_layouts.data(),
			.pushConstantRangeCount = static_cast<uint32_t>(push_constants.size()),
			.pPushConstantRanges = push_constants.data()
		};
		if (vkCreatePipelineLayout(
			g_rhi->device,
			&pipelineLayoutCreateInfo,
			g_rhi->allocator,
			&m_pipeline_layout) != VK_SUCCESS)
			Log::Fatal("Failed to create the Vulkan Pipeline Layout!");
	}

	GRI::GraphicsPipeline::
	GraphicsPipeline()
	{
		Log::Debug("Creating a Graphics Pipeline without Shader Module!");
	}

	GRI::GraphicsPipeline::
	~GraphicsPipeline() noexcept
	{
		vkDestroyPipeline(g_rhi->device, m_handle, g_rhi->allocator);
		m_handle = VK_NULL_HANDLE;
	}

	const VkPipelineVertexInputStateCreateInfo&
	GRI::GraphicsPipeline::
	vertex_input_state()
	{
		static VkPipelineVertexInputStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount	= 0,
			.pVertexBindingDescriptions		= nullptr,
			.vertexAttributeDescriptionCount= 0,
			.pVertexAttributeDescriptions	= nullptr,
		};
		return state;
	}

	const VkPipelineTessellationStateCreateInfo &
	GRI::GraphicsPipeline::
	tessellation_state()
	{
		static VkPipelineTessellationStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
			.patchControlPoints = 0, // Disabled
		};
		return state;
	}

	const VkPipelineInputAssemblyStateCreateInfo&
	GRI::GraphicsPipeline::
	input_assembly_state()
	{
		static VkPipelineInputAssemblyStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};
		return state;
	}

	const VkPipelineViewportStateCreateInfo&
	GRI::GraphicsPipeline::
	viewport_state()
	{
		static VkPipelineViewportStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount	= 1,
			.pViewports		= &m_viewport,
			.scissorCount	= 1,
			.pScissors		= &m_scissor,
		};
		state.pViewports = &m_viewport;
		state.pScissors  = &m_scissor;
		return state;
	}

	const VkPipelineRasterizationStateCreateInfo&
	GRI::GraphicsPipeline::
	rasterization_state()
	{
		static VkPipelineRasterizationStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable		= VK_FALSE, // Fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
			.rasterizerDiscardEnable= VK_FALSE, // if VK_TRUE, then geometry never passes through the rasterizer stage
			.polygonMode			= VK_POLYGON_MODE_FILL,
			.cullMode				= VK_CULL_MODE_BACK_BIT,
			.frontFace				= VK_FRONT_FACE_CLOCKWISE, // Consistent with Unity Engine. (Note Y-Flip)
			.depthBiasEnable		= VK_TRUE,
			.depthBiasConstantFactor= 0.0f, 
			.depthBiasClamp			= 0.0f,
			.depthBiasSlopeFactor	= 0.0f, // This is sometimes used for shadow mapping
			.lineWidth				= 1.0f // Any line thicker than 1.0f requires you to enable the wideLines GPU feature.			
		};
		return state;
	}

	const VkPipelineMultisampleStateCreateInfo&
	GRI::GraphicsPipeline::
	multisampling_state()
	{
		static VkPipelineMultisampleStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable  = VK_FALSE,
			.minSampleShading	  = 1.0f,
			.pSampleMask		  = nullptr,
			.alphaToCoverageEnable= VK_FALSE,
			.alphaToOneEnable	  = VK_FALSE,
		};
		return state;
	}

	const VkPipelineDepthStencilStateCreateInfo&
	GRI::GraphicsPipeline::
	depth_stencil_state()
	{
		static VkPipelineDepthStencilStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			// Depth Test
			.depthTestEnable	= VK_FALSE,
			.depthWriteEnable	= VK_FALSE,
			.depthCompareOp		= VK_COMPARE_OP_LESS, // Keep fragments, which has lower depth
			.depthBoundsTestEnable= VK_FALSE,  // Only keep fragments that fall within the specified depth range.
			// Stencil Test
			.stencilTestEnable	= VK_FALSE,
			.front				= {},
			.back				= {},
			.minDepthBounds		= 0.0,
			.maxDepthBounds		= 1.0,
		};
		return state;
	}

	const VkPipelineColorBlendStateCreateInfo&
	GRI::GraphicsPipeline::	
	color_blend_state()
	{
		static VkPipelineColorBlendStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable		= VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
			.logicOp			= VK_LOGIC_OP_COPY,
			.attachmentCount	= 1,
			.pAttachments		= &m_color_blend,
			.blendConstants		= {0.0f, 0.0f, 0.0f, 0.0f}
		};
		state.pAttachments = &m_color_blend;
		return state;
	}

	const VkPipelineDynamicStateCreateInfo&
	GRI::GraphicsPipeline::
	dynamic_state()
	{
		static VkPipelineDynamicStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 0,
		};
		return state;
	}

	/**
	 * @brief      Wait the next image of Swapchain.
	 * 
	 * @param      signal_semaphore: Semaphores to signal
	 * @param	   signal_fence:	 Fence to signal
	 * @return     void
	 * 
	 * @exception  Unkonwn Exceptions
	 */
	void
	GRI::
	WaitNextFrame(
		VkSemaphore signal_semaphore,
		VkFence		signal_fence) 
	throw (SIGNAL_RECREATE_SWAPCHAIN)
	{
		auto& RT = s_render_targets[g_rhi->swapchain.cursor];
		RT.fence_in_flight.Wait();

		// Compute in each loop
		static uint32_t frame_count = 0;
		frame_count += 1;
		if (RenderTarget::frame_timer.Split().seconds() >= 1)
		{
			RenderTarget::FPS = frame_count / RenderTarget::frame_timer.Split().seconds();
			frame_count = 0;
			RenderTarget::frame_timer.Reset();
		}

		auto result = vkAcquireNextImageKHR(
			g_rhi->device, 
			g_rhi->swapchain,
			std::numeric_limits<uint64_t>::max(),
			signal_semaphore, 
			signal_fence,
			&g_rhi->swapchain.cursor);

		// Judge if recreate Swapchain
		if (VK_ERROR_OUT_OF_DATE_KHR == result ||
			VK_SUBOPTIMAL_KHR		 == result)
		{
			recreate_swapchain();
			throw SIGNAL_RECREATE_SWAPCHAIN{};
		}
		if (result != VK_SUCCESS) Log::Fatal("Failed to retrive the next image of the Vulkan Swap Chain!");	

		RT.fence_in_flight.Reset();
	}

	/**
	 * @brief      Present the current image of Swapchain. (Assure that the final image layout is presentable!)
	 * 
	 * @param      wait_semaphores: Semaphores to wait for (You can create from GRI::Semaphore)
	 * @return     void
	 * 
	 * @exception  SIGNAL_RECREATE_SWAPCHAIN
	 */
	void
	GRI::
	PresentFrame(const std::vector<VkSemaphore>& wait_semaphores)
	throw (SIGNAL_RECREATE_SWAPCHAIN)
	{
		auto& RT = s_render_targets[g_rhi->swapchain.cursor];
		VkSemaphore present_wait_semaphore;

		if (Editor::IsEnabled())
		{
			auto& editorinfo = Editor::Render();
			editorinfo.commandbuffer->Submit(
				{ 
					.wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.wait_semaphores = wait_semaphores,
					.signal_semaphores = {editorinfo.semaphore_editor},
				}, RT.fence_in_flight
			);
			present_wait_semaphore = editorinfo.semaphore_editor;
		}
		else
		{
			auto& current_swapchain_image = g_rhi->swapchain.images[g_rhi->swapchain.cursor];

			VkOffset3D srcBlitOffset{ RT.image->GetExtent().width, RT.image->GetExtent().height, 1 };
			VkOffset3D dstBlitOffset{ g_rhi->swapchain.extent.width, g_rhi->swapchain.extent.height, 1 };
			VkImageBlit blitRegion
			{
				.srcSubresource
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel	= 0,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
				.srcOffsets = {{0,0,0}, srcBlitOffset},
				.dstSubresource
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel	= 0,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
				.dstOffsets = {{0,0,0}, dstBlitOffset},
			};

			VkImageMemoryBarrier barrier_present_to_transfer
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED, // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = current_swapchain_image,
				.subresourceRange
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};

			VkImageMemoryBarrier barrier_transfer_to_present
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = current_swapchain_image,
				.subresourceRange
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};

			RT.commandbuffer->Begin();
			{
				vkCmdPipelineBarrier(*RT.commandbuffer,
									VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
									VK_PIPELINE_STAGE_TRANSFER_BIT,
									0x0,		// Dependency Flags
									0, nullptr,	// Memory Barrier
									0, nullptr,	// Buffer Memory Barrier
									1, &barrier_present_to_transfer);

				auto oldLayout = RT.image->GetLayout();
				RT.image->ConvertLayout(RT.commandbuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

				vkCmdBlitImage(*RT.commandbuffer,
								*RT.image, RT.image->GetLayout(),
								current_swapchain_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								1, &blitRegion,
								VK_FILTER_LINEAR);

				vkCmdPipelineBarrier(*RT.commandbuffer,
									VK_PIPELINE_STAGE_TRANSFER_BIT,
									VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									0x0,		// Dependency Flags
									0, nullptr,	// Memory Barrier
									0, nullptr,	// Buffer Memory Barrier
									1, &barrier_transfer_to_present);

				RT.image->ConvertLayout(RT.commandbuffer, oldLayout);
			}
			RT.commandbuffer->End();

			RT.commandbuffer->Submit
			({
				.wait_stages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.wait_semaphores   = wait_semaphores,
				.signal_semaphores = {RT.semaphore_ready},
			}, RT.fence_in_flight);
			
			present_wait_semaphore = RT.semaphore_ready;
		}
		
		// Submit Present
		VkPresentInfoKHR presentInfo
		{
			.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores	= &present_wait_semaphore,
			.swapchainCount		= 1,
			.pSwapchains		= &g_rhi->swapchain.handle,
			.pImageIndices		= &g_rhi->swapchain.cursor,
			.pResults			= nullptr // It is not necessary if you are only using a single swap chain
		};

		auto result = vkQueuePresentKHR(GetQueue(QueueFamilyType_Present), &presentInfo);

		if (VK_ERROR_OUT_OF_DATE_KHR == result ||
			VK_SUBOPTIMAL_KHR		 == result)
		{
			recreate_swapchain();
			throw SIGNAL_RECREATE_SWAPCHAIN{};
		}

		if (result != VK_SUCCESS) Log::Fatal("Failed to present the Vulkan Swap Chain!");
		g_rhi->swapchain.cursor = (g_rhi->swapchain.cursor + 1) % g_rhi->swapchain.images.size();
	}

	/**
	 * @brief      Wait the Global Logical Device to idle.
	 * 
	 * @param      void
	 * @return     void
	 * 
	 * @exception  No Exceptions
	 */
	void
	GRI::
	WaitDeviceIdle()
	{
		vkDeviceWaitIdle(g_rhi->device);
	}

	void
	GRI::
	ClearScreen(std::shared_ptr<CommandBuffer> commandbuffer, const VkClearColorValue& clear_color)
	{
		//[WARN] It is invalid to issue this call inside an active VkRenderPass!
		auto& screen = s_render_targets[g_rhi->swapchain.cursor].image;
		screen->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		auto subsrcrange = screen->GetSubresourceRange();
		vkCmdClearColorImage(*commandbuffer,
			*screen,
			screen->GetLayout(),
			&clear_color,
			1, &subsrcrange);
		screen->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
	}

	uint32_t
	GRI::GetFPS()
	{
		return RenderTarget::FPS;
	}

	void
	GRI::
	Screenshot(std::shared_ptr<Texture> output)
	{
		ALBEDO_ASSERT(VK_IMAGE_USAGE_TRANSFER_DST_BIT & output->m_settings.usage);

		auto commandbuffer = 
			GetGlobalCommandPool(
			CommandPoolType_Transient,
			QueueFamilyType_Graphics)
			->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });

		uint32_t prev_cursor = (GetRenderTargetCursor() + (g_rhi->swapchain.images.size() - 1)) % g_rhi->swapchain.images.size();
		auto& PrevRT = s_render_targets[prev_cursor];
		PrevRT.fence_in_flight.Wait();
		PrevRT.fence_in_flight.Reset();

		commandbuffer->Begin();
		{
			PrevRT.image->Blit(commandbuffer, output);
		}
		commandbuffer->End();
		commandbuffer->Submit(
			{
				.wait_stages  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.wait_semaphores	= {},
				.signal_semaphores	= {},
			}, PrevRT.fence_in_flight);
		PrevRT.fence_in_flight.Wait();
	}

} // namespace Albedo

namespace
{
	void
	VMA::
	Create()
	{
		VmaAllocatorCreateInfo vmaAllocatorCreateInfo
		{
			.flags = 0x0,							//VmaAllocatorCreateFlagBits
			.physicalDevice = g_rhi->GPU,
			.device = g_rhi->device,
			.preferredLargeHeapBlockSize = 0,		// 0 means Default (256MiB)
			.pAllocationCallbacks = g_rhi->allocator,
			.pDeviceMemoryCallbacks = nullptr,
			.instance = g_rhi->instance,
			.vulkanApiVersion  = VULKAN_API_VERSION // VMA Default API Version 1.0
		};
		
		if (vmaCreateAllocator(&vmaAllocatorCreateInfo, &handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the VMA (Vulkan Memory Allocator)!");
	}

	void
	VMA::
	Destroy()
	{
		vmaDestroyAllocator(handle);
		handle = VK_NULL_HANDLE;
	}

	void
	KTX::
	Create()
	{
		context = ktxVulkanDeviceInfo_Create(
			g_rhi->GPU,
			g_rhi->device,
			g_rhi->device.queue_families.transfer.queues.front(),
			*GRI::GetGlobalCommandPool(CommandPoolType_Resettable, QueueFamilyType_Transfer),
			g_rhi->allocator);
	}
	
	void
	KTX::
	Destroy()
	{
		ktxVulkanDeviceInfo_Destroy(context);
	}

	void
	RenderTarget::
	Initialize()
	{
		s_render_targets.resize(g_rhi->swapchain.images.size());

		auto commandbuffer = GRI::GetGlobalCommandPool(
			CommandPoolType_Transient,
			QueueFamilyType_Graphics)
			->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });

		// Convert Swapchain Image Layout
		VkImageMemoryBarrier barrier
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = 0x0,
			.dstAccessMask = 0x0,
			.oldLayout	   = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.subresourceRange
			{
				.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel	= 0,
				.levelCount		= 1,
				.baseArrayLayer = 0,
				.layerCount		= 1,
			}
		};

		commandbuffer->Begin();
		{
			// Create Color Attachment
			for (size_t i = 0; i < s_render_targets.size(); ++i)
			{
				auto& RT = s_render_targets[i];
				RT.image = GRI::Texture::Create(GRI::Texture::CreateInfo
					{
						.aspect = VK_IMAGE_ASPECT_COLOR_BIT,
						.usage  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
								  VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
						.format = GRI::GetRenderTargetFormat(),
						.extent = {g_rhi->swapchain.extent.width, g_rhi->swapchain.extent.height, 1},
						.mipLevels	 = 1,
						.arrayLayers = 1,
						.samples = VK_SAMPLE_COUNT_1_BIT,
						.tiling  = VK_IMAGE_TILING_OPTIMAL,
					});
				RT.image->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
				RT.commandbuffer = GRI::GetGlobalCommandPool(
					CommandPoolType_Resettable,
					QueueFamilyType_Graphics)
					->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });

				// Convert Swapchain Image Layout
				barrier.image = g_rhi->swapchain.images[i];
				vkCmdPipelineBarrier(
					*commandbuffer,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					0x0,		// Dependency Flags
					0, nullptr,	// Memory Barrier
					0, nullptr,	// Buffer Memory Barrier
					1, &barrier);
			}
			// Create ZBuffer(Shared)
			RenderTarget::zbuffer = GRI::Texture::Create(GRI::Texture::CreateInfo
					{
						.aspect = VK_IMAGE_ASPECT_DEPTH_BIT,
						.usage  = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
						.format = VK_FORMAT_D32_SFLOAT,
						.extent = {g_rhi->swapchain.extent.width, g_rhi->swapchain.extent.height, 1},
						.mipLevels	 = 1,
						.arrayLayers = 1,
						.samples = VK_SAMPLE_COUNT_1_BIT,
						.tiling  = VK_IMAGE_TILING_OPTIMAL,
					});
			RenderTarget::zbuffer->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}
		commandbuffer->End();
		GRI::Fence fence{ FenceType_Unsignaled };
		commandbuffer->Submit({}, fence);
		fence.Wait();
	}

	void
	RenderTarget::
	Destroy()
	{
		s_render_targets.clear();
		RenderTarget::zbuffer.reset();
	}

} // namespace