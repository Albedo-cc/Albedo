#pragma once
/**
 * @file       RHI.h
 * @brief      GRI(Graphics Render Interface), User-level interface.
 * @author     LJYC(https://github.com/LJYC-ME)
 */

// Header Mess

#include "common.h"

namespace Albedo { namespace Graphics
{

	struct RHICreateInfo
	{
		const char* app_name    = nullptr;
		uint32_t	app_version = 0;
		GLFWwindow* app_window  = nullptr;

		PFN_vkDebugUtilsMessengerCallbackEXT msg_callback = nullptr;
	};

    class RHI
    {
	public: // User-level Interfaces (e.g. Manage Global GPU Resource)
		static void RegisterGlobalDescriptorSetLayout(std::string id, std::shared_ptr<DescriptorSetLayout> descriptor_set_layout);
		static void RegisterGlobalSampler(std::string id, std::shared_ptr<Sampler> sampler);
		static void RegisterGlobalTexture(std::string id, std::shared_ptr<Texture> texture);

		static auto GetGlobalDescriptorSetLayout(const std::string& id) -> std::shared_ptr<DescriptorSetLayout>;
		static auto GetGlobalDescriptorPool(std::thread::id thread_id = std::this_thread::get_id()) -> std::shared_ptr<DescriptorPool>;
		static auto GetGlobalCommandPool(CommandPoolType type, QueueFamilyType queue, std::thread::id thread_id = std::this_thread::get_id()) -> std::shared_ptr<CommandPool>;
		static auto GetGlobalSampler(const std::string& id) -> std::shared_ptr<Sampler>;
		static auto GetGlobalTexture(const std::string& id) -> std::shared_ptr<Texture>;

		static auto GetQueue(QueueFamilyType queue_family, uint32_t index = 0) -> VkQueue;
		static auto GetQueueFamilyIndex(QueueFamilyType queue_family) -> uint32_t;
		static auto GetCurrentRenderTarget() -> std::shared_ptr<const Texture>;
		static auto GetZBuffer()			 -> std::shared_ptr<const Texture>;
		static auto GetRenderTargetCount()	 -> size_t;
		static auto GetRenderTargetCursor()	 -> uint32_t;
		static auto GetRenderTargetFormat()	 -> VkFormat;

		static auto GetFPS() -> uint32_t;
		static void Screenshot(std::shared_ptr<Texture> output);

		static auto MakeID(const std::vector<VkDescriptorType>& types_in_order) -> std::string;
		static auto PadUniformBufferSize(size_t original_size) -> size_t;

	public: // Developer-level Interface
		class SIGNAL_RECREATE_SWAPCHAIN : public std::exception {};
		static void WaitNextFrame(VkSemaphore signal_semaphore, VkFence signal_fence) throw (SIGNAL_RECREATE_SWAPCHAIN);
		static void PresentFrame(const std::vector<VkSemaphore>& wait_semaphores) throw (SIGNAL_RECREATE_SWAPCHAIN);
		static void WaitDeviceIdle();
		static void ClearScreen(std::shared_ptr<CommandBuffer> commandbuffer, const VkClearColorValue& clear_color);

	public:
		static void Initialize(const RHICreateInfo& createinfo);
        static void Terminate() noexcept;
		
	private:
		static void recreate_swapchain();

    private:
        RHI()                      = delete;
        RHI(const RHI&)            = delete;
        RHI(RHI&&)                 = delete;
        RHI& operator=(const RHI&) = delete;
        RHI& operator=(RHI&&)      = delete;
    };

}} // namespace Albedo::Graphics