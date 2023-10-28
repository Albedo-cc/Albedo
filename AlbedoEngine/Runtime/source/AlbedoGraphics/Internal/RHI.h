#pragma once
/**
 * @file       RHI.h
 * @brief      RHI(Render Handware Interface), Developer-level interface, it is recommanded that use GRI.h
 * @author     LJYC(https://github.com/LJYC-ME)
 */

// Header Mess
#include <AlbedoCore/Norm/assert.h>
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <optional>
#include <memory>
#include <vector>

// Predeclartion
typedef struct GLFWwindow GLFWwindow;
typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Albedo
{    
	// Internal Global Context Interface
	class RHI; extern std::unique_ptr<RHI> g_rhi;
	struct RHICreateInfo
    {
        const char* app_name    = nullptr;
        uint32_t	app_version = 0;
        GLFWwindow* app_window  = nullptr;

        PFN_vkDebugUtilsMessengerCallbackEXT msg_callback = nullptr;
    };

	constexpr auto ALBEDO_RHI_NAME          =   "Albedo RHI";
	constexpr auto ALBEDO_RHI_VERSION		=	VK_MAKE_API_VERSION(0,0,1,0);
	constexpr auto VULKAN_API_VERSION 		=   VK_API_VERSION_1_3;

#ifdef NDEBUG
	constexpr bool IS_DEBUG_MODE = false;
#else
	constexpr bool IS_DEBUG_MODE = true; // Enable Debug Messenger & Save Pipeline Cache
#endif

	class RHI
	{
		friend class GRI; // Initialze by GRI
	public:
		const VkAllocationCallbacks*	allocator  { nullptr };

		struct Instance
		{
			const char*					app_name;
			uint32_t					app_version;
			VkInstance					handle		{ VK_NULL_HANDLE };	
			
			std::vector<const char*>	layers;
			std::vector<const char*>	extensions;
			operator VkInstance() const { return handle; }
		}instance{};

		struct Messenger
		{
			VkDebugUtilsMessengerEXT			 handle;
			PFN_vkDebugUtilsMessengerCallbackEXT callback = default_messenger_callback;
			operator VkDebugUtilsMessengerEXT() const { return handle; }
		}messenger; // Create & Destroy with the Instance

		struct Window
		{
			GLFWwindow* pointer;
			operator GLFWwindow* () const { return pointer; }
		}window{};

		struct Surface
		{
			VkSurfaceKHR                        handle{ VK_NULL_HANDLE };
			std::vector<VkSurfaceFormatKHR>		formats;
			std::vector<VkPresentModeKHR>		present_modes;
			operator VkSurfaceKHR() const { return handle; }
		}surface{};

		struct PhysicalDevice
		{
			VkPhysicalDevice					handle{ VK_NULL_HANDLE };
			VkPhysicalDeviceFeatures			features;
			VkPhysicalDeviceProperties			properties;
			VkPhysicalDeviceMemoryProperties	memory_properties;
			operator VkPhysicalDevice() const { return handle; }
		}GPU{};

		struct LogicalDevice
		{
			VkDevice					handle{ VK_NULL_HANDLE };
			struct QueueFamily
			{
				struct Queue
				{
					VkQueue handle		{ VK_NULL_HANDLE };
					float	priority	{1.0};
					operator VkQueue() const { return handle; }
				};
				std::vector<Queue>		queues;
				std::optional<uint32_t>	index;
				QueueFamily() = delete;
				QueueFamily(const std::vector<float>& priorities): queues(priorities.size())
				{ for (int i = 0; i < queues.size(); ++i) queues[i].priority = priorities[i]; }
				operator uint32_t() { ALBEDO_ASSERT(index.has_value()); return index.value(); }
			};
			struct
			{
				QueueFamily			graphics	{ {1.0} }; // 1 Queue
				QueueFamily			present		{ {1.0} }; // 1 Queue (Assert == graphics)
				QueueFamily			transfer	{ {1.0} }; // 1 Queue
				QueueFamily			compute		{ {1.0} }; // 1 Queue
				//QueueFamily		sparsebinding; // Do not need to support now
			}queue_families;

			std::vector<const char*>	extensions;
			operator VkDevice() const { return handle; }
		}device{};

		struct VMA
		{
			VmaAllocator		handle{ VK_NULL_HANDLE };
			operator VmaAllocator() const { return handle; }
		}vma;

		struct Swapchain
		{
			VkSwapchainKHR			handle			{ VK_NULL_HANDLE };
			std::vector<VkImage>	images;			// Size: clamp(minImageCount + 1, maxImageCount)
			std::vector<VkImageView>image_views;
			VkExtent2D				extent;			// Current extent
			uint32_t				cursor{ 0 };	// Current image index for rendering
			VkFormat				format			= VK_FORMAT_B8G8R8A8_SRGB;
			VkColorSpaceKHR			color_space		= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			VkPresentModeKHR		present_mode	= VK_PRESENT_MODE_MAILBOX_KHR;
			VkFormat				zbuffer_format  = VK_FORMAT_D32_SFLOAT;
			VkImageTiling			zbuffer_tiling  = VK_IMAGE_TILING_OPTIMAL;
			
			operator VkSwapchainKHR() const { return handle; }
		}swapchain;

		struct PipelineCache
		{
			VkPipelineCache handle	   {VK_NULL_HANDLE};
			const char*		file	   = "cache/pipelines.cache";
			bool			is_expired = false;
			operator VkPipelineCache() const { return handle; }
		}pipeline_cache;

	private:
		// Initialization & Destroy
		void create_instance();				void destroy_instance();

		void create_surface();				void destroy_surface();

		void create_physical_device();		// Implicitly destroyed when the VkInstance is destroyed.
		bool check_physical_device_features_support();
		bool check_physical_device_queue_families_support();
		bool check_physical_device_extensions_support();
		bool check_physical_device_surface_support();

		void create_logical_device();		void destroy_logical_device();

		void create_memory_allocator();		void destroy_memory_allocator();

		void create_swapchain();			void destroy_swapchain();
		void recreate_swapchain();
		bool check_swap_chain_image_format_support();
		bool check_swap_chain_depth_format_support();
		bool check_swap_chain_present_mode_support();

		void create_pipeline_cache();		void destroy_pipeline_cache();

	public:
		RHI();
		~RHI() noexcept;

	private:
		void Initialize(const RHICreateInfo& createinfo);
		void Terminate() noexcept;
        RHI(const RHI&)				= delete;
        RHI(RHI&&)					= delete;
        RHI& operator=(const RHI&)	= delete;
        RHI& operator=(RHI&&)		= delete;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL
			default_messenger_callback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData);
	};

} // namespace Albedo