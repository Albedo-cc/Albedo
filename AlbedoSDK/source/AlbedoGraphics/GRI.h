#pragma once
/**
 * @file       GRI.h
 * @brief      GRI(Graphics Render Interface), User-level interface.
 * @author     LJYC(https://github.com/LJYC-ME)
 */

 // Header Mess
#include "GRIEnums.h"

#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <functional>
#include <string_view>

// Predeclartion
typedef struct GLFWwindow GLFWwindow;
typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VmaAllocation_T* VmaAllocation;

namespace Albedo
{
    struct GRICreateInfo
    {
        const char* app_name    = nullptr;
        uint32_t	app_version = 0;
        GLFWwindow* app_window  = nullptr;

        PFN_vkDebugUtilsMessengerCallbackEXT msg_callback = nullptr;
    };

    class GRI
    {
	//[GRI OBJECTS]----------------------------------------------------------------------------------------------------------------
	/*Command*/			class CommandPool; class CommandBuffer; class TransientCommandBuffer; class AutoResetCommandBuffer;
	/*Synchronization*/	class Fence; class Semaphore;
	/*Memory*/			class Shader; class Buffer; class Image;
	/*Interface Class*/	class RenderPass; class GraphicsPipeline;
	/*Descriptor*/		class DescriptorSetLayout; class DescriptorPool; class DescriptorSet;
						class Sampler; class Texture;
	//[TIPS]-----------------------------------------------------------------------------------------------------------------------
	// 1. For better Interface Compatibility, you should create GRI objects via [Class]::Create(...).
	//    Importantly, DO NOT create GRI objects without [Class]::Create(...) by yourself!
	//-----------------------------------------------------------------------------------------------------------------------------
	
	public: // User-level Interfaces
		static void RegisterGlobalDescriptorSetLayout(std::string id, std::shared_ptr<DescriptorSetLayout> descriptor_set_layout);
		static void RegisterGlobalSampler(std::string id, std::shared_ptr<Sampler> sampler);

		static auto GetGlobalDescriptorSetLayout(std::string_view id) -> std::shared_ptr<GRI::DescriptorSetLayout>;
		static auto GetGlobalDescriptorPool(std::thread::id thread_id = std::this_thread::get_id()) -> std::shared_ptr<GRI::DescriptorPool>;
		static auto GetGlobalCommandPool(CommandPoolType type, QueueFamilyType queue, std::thread::id thread_id = std::this_thread::get_id()) -> std::shared_ptr<GRI::CommandPool>;
		
		static auto GetQueue(QueueFamilyType queue_family, uint32_t index = 0) -> VkQueue;
		static auto GetQueueFamilyIndex(QueueFamilyType queue_family) -> uint32_t;
		static auto GetCurrentRenderTarget() -> const std::shared_ptr<const GRI::Image>&;
		static auto GetRenderTargetCount()	 -> size_t;
		static auto GetRenderTargetCursor()	 -> uint32_t;
		static auto GetRenderTargetFormat()	 -> VkFormat;
		static auto GetZBuffer()			 -> const std::shared_ptr<const GRI::Image>&;

		static void PushPreframeTask(std::shared_ptr<CommandBuffer> commandbuffer, std::thread::id thread_id = std::this_thread::get_id());

		static void ScreenShot(std::shared_ptr<Image> output);
		static void ScreenShot(std::shared_ptr<Texture> output);

	public: // Developer-level Interface
		class SIGNAL_RECREATE_SWAPCHAIN : public std::exception {};
		static void WaitNextFrame(VkSemaphore signal_semaphore, VkFence signal_fence) throw (SIGNAL_RECREATE_SWAPCHAIN);
		static void PresentFrame(const std::vector<VkSemaphore>& wait_semaphores) throw (SIGNAL_RECREATE_SWAPCHAIN);
		static void WaitDeviceIdle();
		static void ClearScreen(std::shared_ptr<CommandBuffer> commandbuffer, const VkClearColorValue& clear_color);

    public: // Initialize & Terminate
        static void Initialize(const GRICreateInfo& createinfo);
        static void Terminate() noexcept;

	private:
		class GRIObject
		{
			friend class GRI;
			enum Tag : uint8_t {None = 0, Global = 1 << 0, };
			void add_gri_tag(Tag tag) { m_gri_tags = Tag(m_gri_tags | tag); }
			Tag m_gri_tags{ None };
		};

	public: // GRI Objects
        class Fence final
			:public GRIObject
		{
			friend class GRI;
		public:
			void Wait(uint64_t timeout = std::numeric_limits<uint64_t>::max());
			void Reset();
			auto IsReady() -> bool;

		public:
			static inline auto Create(FenceType type) { return Fence{type}; }
			Fence() = delete;
			Fence(FenceType type);
			~Fence() noexcept;
			operator VkFence() const { return m_handle; }

		private:
			VkFence m_handle{ VK_NULL_HANDLE };
		};

		class Semaphore final
			:public GRIObject
		{
			friend class GRI;
		public:
			static inline auto Create(SemaphoreType type) { return Semaphore{type}; }
			Semaphore() = delete;
			Semaphore(SemaphoreType type);
			~Semaphore() noexcept;
			operator VkSemaphore() const { return m_handle; }

		private:
			VkSemaphore m_handle{ VK_NULL_HANDLE };
		};

		class CommandBuffer
			:public GRIObject
		{
			friend class GRI;
		public:
			struct CreateInfo
			{
				CommandBufferLevel    level;
			};

			struct SubmitInfo
			{
				VkPipelineStageFlags			  wait_stages{ 0 };
				VkFence							  signal_fence{ VK_NULL_HANDLE };
				std::vector<VkSemaphore>		  wait_semaphores;
				std::vector<VkSemaphore>		  signal_semaphores;
			};

		public:
			virtual void Begin();
			virtual void End();
			virtual void Submit(const SubmitInfo& submitinfo);
			auto IsRecording() const -> bool { return m_is_recording; }
			auto GetSettings() const -> const CreateInfo& { return m_settings; }
			operator VkCommandBuffer() const { return m_handle; }
	
		public:
			CommandBuffer() = delete;
			CommandBuffer(std::shared_ptr<CommandPool> command_pool, const CreateInfo& createinfo);
			virtual ~CommandBuffer() noexcept;

		protected:
			VkCommandBuffer				 m_handle{ VK_NULL_HANDLE };
			CreateInfo					 m_settings;
			std::shared_ptr<CommandPool> m_parent;
			bool						 m_is_recording = false;
		};

		class TransientCommandBuffer final : public CommandBuffer
		{
			friend class GRI;
		public:
			virtual void Begin() override;
			//virtual void End()  override;
			//virtual void Submit(const CommandBuffer::SubmitInfo& submitinfo) override;

		public:
			TransientCommandBuffer() = delete;
			TransientCommandBuffer(std::shared_ptr<CommandPool> command_pool, const CreateInfo& createinfo):
				CommandBuffer{command_pool, createinfo}{}
		};

		class AutoResetCommandBuffer final : public CommandBuffer
		{
			friend class GRI;
		public:
			virtual void Begin() override;
			//virtual void End()  override;
			//virtual void Submit(const CommandBuffer::SubmitInfo& submitinfo) override;

		public:
			AutoResetCommandBuffer() = delete;
			AutoResetCommandBuffer(std::shared_ptr<CommandPool> command_pool, const CreateInfo& createinfo):
				CommandBuffer{command_pool, createinfo}{}
		};

		class CommandPool final:
			public GRIObject,
			public std::enable_shared_from_this<CommandPool>
		{
			friend class GRI;
		public:
			struct CreateInfo
			{
				QueueFamilyType	queue_family;
				CommandPoolType	type;
			};

		public:
			auto AllocateCommandBuffer(const CommandBuffer::CreateInfo& createinfo = {}) -> std::shared_ptr<CommandBuffer>;
			auto GetSettings() const -> const CreateInfo& { return m_settings; }
			operator VkCommandPool() const { return m_handle; }

		public:
			static inline auto Create(const GRI::CommandPool::CreateInfo& createinfo)
			{ return std::make_shared<CommandPool>(createinfo); }
			CommandPool() = delete;
			CommandPool(const CreateInfo& createinfo);
			~CommandPool() noexcept;

		private:
			VkCommandPool m_handle{ VK_NULL_HANDLE };
			CreateInfo	  m_settings;
		};

		class DescriptorSetLayout final
			:public GRIObject
		{
			friend class GRI;
		public:
			auto GetBinding(uint32_t index) const -> const VkDescriptorSetLayoutBinding&
			{assert(m_bindings.size() > index); return m_bindings[index]; }
			operator VkDescriptorSetLayout() const { return m_handle; }

		public:
			static inline auto Create(std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings)
			{ return std::make_shared<DescriptorSetLayout>(descriptor_bindings); }
			DescriptorSetLayout() = delete;
			DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings);
			~DescriptorSetLayout() noexcept;

		private:
			VkDescriptorSetLayout m_handle{ VK_NULL_HANDLE };
			std::vector<VkDescriptorSetLayoutBinding> m_bindings;
		};

		class DescriptorPool final:
			public GRIObject,
			public std::enable_shared_from_this<DescriptorPool>
		{
			friend class GRI;
		public:
			auto AllocateDescriptorSet(std::shared_ptr<DescriptorSetLayout> layout) -> std::shared_ptr<DescriptorSet>;
			operator VkDescriptorPool() const { return m_handle; }

		public:
			static inline auto Create(const std::vector<VkDescriptorPoolSize>& pool_size, uint32_t limit_max_sets)
			{ return std::make_shared<DescriptorPool>(pool_size, limit_max_sets); }
			DescriptorPool() = delete;
			DescriptorPool(const std::vector<VkDescriptorPoolSize>& pool_size, uint32_t limit_max_sets);
			~DescriptorPool() noexcept;

		private:
			VkDescriptorPool m_handle{ VK_NULL_HANDLE };
		};

		static void UpdateDescriptorSets(const std::vector<VkWriteDescriptorSet>& updateinfo);
		class DescriptorSet final
			:public GRIObject
		{
			friend class GRI;
		public:
			// GRI::UpdateDescriptorSets({SetA.WriteBuffer(...), SetB.WriteImage(...), ...}
			auto WriteBuffer(uint32_t binding, std::shared_ptr<Buffer> buffer) -> VkWriteDescriptorSet;
			auto WriteTexture(uint32_t binding, std::shared_ptr<Texture> texture)  -> VkWriteDescriptorSet;

			operator VkDescriptorSet() const { return m_handle; }

		public:
			DescriptorSet() = delete;
			DescriptorSet(std::shared_ptr<DescriptorPool> parent, std::shared_ptr<DescriptorSetLayout> layout);
			~DescriptorSet() noexcept;

		private:
			std::shared_ptr<DescriptorPool>		 m_parent;
			VkDescriptorSet						 m_handle{ VK_NULL_HANDLE };
			std::shared_ptr<DescriptorSetLayout> m_layout;
		};

		class Shader final
			:public GRIObject
		{
			friend class GRI;
		public:
			auto GetSize() const -> VkDeviceSize { return m_code.size(); }
			auto GetCode() const -> const std::vector<char>& { return m_code; }
			auto GetType() const -> ShaderType	 { return m_type; }
			auto GetStage()const -> VkShaderStageFlagBits { return static_cast<VkShaderStageFlagBits>(m_type); }
			operator VkShaderModule() const { return m_handle; }

		public:
			static inline auto Create(ShaderType type, std::vector<char> code)
			{ return std::make_shared<GRI::Shader>(type, code); }
			Shader() = delete;
			Shader(ShaderType type, std::vector<char> code);
			~Shader() noexcept;

		private:
			VkShaderModule m_handle{ VK_NULL_HANDLE };
			ShaderType	   m_type;
			std::vector<char> m_code;
		};
		
		class Buffer final
			:public GRIObject
		{
			friend class GRI;
		public:
			enum Property { Readable = 1 << 0, Writable = 1 << 1, Persistent = 1 << 2, };
			struct CreateInfo // Create by GRI
			{
				VkDeviceSize	   size;
				VkBufferUsageFlags usage;
				VkSharingMode      mode;
				Property		   properties;
			};

		public:
			void Write(void* data); // The buffer must be mapping-allowed and writable
			auto Access() -> void*; // If the buffer is persistently mapped, you can access its memory directly

			struct CopyInfo
			{
				VkDeviceSize range		   = 0; // ALL == 0
				VkDeviceSize source_offset = 0;
				VkDeviceSize target_offset = 0;
			};
			void CopyTo(std::shared_ptr<CommandBuffer> commandbuffer, std::shared_ptr<Buffer> target, const CopyInfo& copyinfo = {}) const;
			void CopyFrom(std::shared_ptr<CommandBuffer> commandbuffer, std::shared_ptr<Buffer> target, const CopyInfo& copyinfo = {});
		
			auto GetSize()		const ->VkDeviceSize;
			operator VkBuffer() const { return m_handle; }

		public:
			static inline auto Create(GRI::Buffer::CreateInfo createinfo)
			{ return std::make_shared<GRI::Buffer>(createinfo); }
			Buffer(const CreateInfo& createinfo);	
			~Buffer() noexcept;
			Buffer() = delete;

		private:
			VkBuffer	  m_handle	   { VK_NULL_HANDLE };
			VmaAllocation m_allocation { VK_NULL_HANDLE };	
		};

		class Image final
			:public GRIObject
		{
			friend class GRI;
		public:
			struct CreateInfo // Create by GRI
			{
				VkImageAspectFlags	aspect;
				VkImageUsageFlags	usage;
				VkFormat			format;
				VkExtent3D			extent;
				uint32_t			mipLevels	= 1;
				uint32_t            arrayLayers = 1;
				VkSampleCountFlagBits   samples   = VK_SAMPLE_COUNT_1_BIT;
				VkImageTiling			tiling	  = VK_IMAGE_TILING_OPTIMAL;
			};

		public:
			void Write(std::shared_ptr<CommandBuffer> commandbuffer, std::shared_ptr<Buffer> data);
			void Blit(std::shared_ptr<CommandBuffer> commandbuffer, std::shared_ptr<Image>  target) const;
			void ConvertLayout(std::shared_ptr<CommandBuffer> commandbuffer, VkImageLayout target_layout);

			auto GetLayout()			const -> VkImageLayout{ return m_layout; }
			auto GetView()				const -> VkImageView { return m_view; }
			auto GetFormat()			const -> VkFormat { return m_settings.format; }
			auto GetExtent()			const -> const VkExtent3D& { return m_settings.extent; }
			auto GetMipmapLevels()		const -> uint32_t { return m_settings.mipLevels; }
			auto GetArraryLevels()		const -> uint32_t { return m_settings.arrayLayers; }
			auto GetSubresourceRange()	const ->VkImageSubresourceRange;
			auto GetSubresourceLayers() const ->VkImageSubresourceLayers;
			auto GetSize()				const ->VkDeviceSize;
			operator VkImage()			const { return m_handle; }

			auto HasStencil()		const -> bool;
			auto HasMipmap()		const -> bool { return m_settings.mipLevels - 1; }

		public:
			static inline auto Create(GRI::Image::CreateInfo createinfo)
			{ return std::make_shared<GRI::Image>(createinfo); }
			Image(CreateInfo createinfo);
			~Image() noexcept;

		private:
			void fill_convert_layout_info(VkImageLayout source_layout, VkImageLayout target_layout, 
				VkImageMemoryBarrier& barrier, VkPipelineStageFlags& from_stage, VkPipelineStageFlags& to_stage) const;

		private:
			VkImage			m_handle		{ VK_NULL_HANDLE };
			VkImageView		m_view			{ VK_NULL_HANDLE };
			VkImageLayout	m_layout		{ VK_IMAGE_LAYOUT_UNDEFINED };
			CreateInfo		m_settings;
			VmaAllocation	m_allocation	{ VK_NULL_HANDLE };
		};

		class Sampler
			:public GRIObject
		{
			friend class GRI;
		public:
			struct CreateInfo
			{
				// General
				union ImageWrap
				{
					VkSamplerAddressMode modes[3] = { VK_SAMPLER_ADDRESS_MODE_REPEAT };
					struct { VkSamplerAddressMode U, V, W; }; // If V or W is 0(REPEAT), they use U mode.
				} wrap_mode;

				// Advanced
				union ImageFilter
				{
					VkFilter modes[2] = { VK_FILTER_LINEAR, VK_FILTER_LINEAR };
					struct { VkFilter magnify, minimize; };
				} filter;
				VkBorderColor		 border_color = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				VkSamplerMipmapMode  mipmap_mode  = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				VkCompareOp			 compare_mode = VK_COMPARE_OP_NEVER;
				VkBool32             enable_anisotropy = VK_TRUE;
			};

		public:
			operator VkSampler() const { return m_handle; }

		public:
			static inline auto Create(GRI::Sampler::CreateInfo createinfo)
			{ return std::make_shared<GRI::Sampler>(createinfo); }
			Sampler(CreateInfo createinfo);
			Sampler() = delete;
			~Sampler() noexcept;

		private:
			VkSampler  m_handle{ VK_NULL_HANDLE };
			CreateInfo m_setting;
		};

		class Texture
			:public GRIObject
		{
			friend class GRI;
			friend class DescriptorSet;
		public:
			operator VkImage() const { return *m_image; }

		public:
			static inline auto Create(std::shared_ptr<Image> image, std::shared_ptr<Sampler> sampler)
			{ return std::make_shared<Texture>(image, sampler); }
			Texture(std::shared_ptr<Image> image, std::shared_ptr<Sampler> sampler);
			Texture() = delete;
			~Texture() noexcept;

		private:
			std::shared_ptr<Image>   m_image;
			std::shared_ptr<Sampler> m_sampler;
		};

		class RenderPass
		{
			friend class GRI;
		public:
			virtual void Begin(std::shared_ptr<CommandBuffer> commandbuffer);
			virtual void End(std::shared_ptr<CommandBuffer> commandbuffer);

			auto	 GetName()	     const -> std::string_view { return m_name; }
			auto	 GetRenderArea() const -> const VkRect2D& { return m_render_area; }
			auto	 GetPriority()   const -> uint32_t { return m_priority; }
			auto	 SeachSubpass(std::string_view name) const throw(std::runtime_error) -> uint32_t;
			operator VkRenderPass()  const { return m_handle; }

		protected:
			enum SystemTarget
			{
				ST_Color	= 0,
				ST_ZBuffer	= 1,
				MAX_SYSTEM_TARGET,
			};
			auto get_system_target_reference(SystemTarget target) -> VkAttachmentReference;

			struct AttachmentSetting
			{
				VkAttachmentDescription description;
				VkClearValue			clearColor{{1,0,1,1}};
			};

			struct FramebufferSetting
			{
				std::vector<VkImageView> render_targets;
				// {width, height} == m_render_area
				// layer = 1
			};

			struct SubpassSetting
			{
				std::string							 name;
				std::unique_ptr<GraphicsPipeline>    pipeline;
				// Attachments
				std::vector<VkAttachmentReference>   input_attachments;
				std::vector<VkAttachmentReference>   color_attachments;
				//std::vector<VkAttachmentReference>   resolve_attachments;
				//std::vector<VkAttachmentReference>   preserve_attachments;
				std::optional<VkAttachmentReference> depth_stencil_attachment;
				// Dependency
				VkPipelineStageFlags				 source_stage_mask		 = 0x0;
				VkAccessFlags						 source_access_mask		 = 0x0;
				VkPipelineStageFlags				 destination_stage_mask  = 0x0;
				VkAccessFlags						 destination_access_mask = 0x0;
			};

		protected:
			auto add_attachment(AttachmentSetting setting) -> uint32_t;
			auto add_subpass(SubpassSetting setting)	   -> uint32_t;
			void add_framebuffer(FramebufferSetting setting);
			void BUILD_ALL(); // Call after add_subpass() & add_attachment()
			void BUILD_SELF();
			void BUILD_SUBPASSES();
			void BUILD_FRAMEBUFFERS();

		public:
			RenderPass() = delete;
			RenderPass(std::string name, uint32_t priority);
			virtual ~RenderPass() noexcept;

		protected:
			std::string   m_name;
			VkRenderPass  m_handle	   { VK_NULL_HANDLE };
			uint32_t	  m_priority   { 0 }; // Less is more
			VkRect2D	  m_render_area; // {{0,0}, {swapchain.extent}}
			std::vector<SubpassSetting> m_subpasses;
			struct FrameBuffer
			{
				VkFramebuffer			 handle = VK_NULL_HANDLE;
				std::vector<VkImageView> render_targets;
				operator VkFramebuffer() const { return handle; }
			};
			std::vector<FrameBuffer> m_framebuffers;
			struct AttachmentSet
			{
				std::vector<VkImageView>			 images;
				std::vector<VkAttachmentDescription> descriptions;
				std::vector<VkClearValue>			 clear_colors;
			};
			AttachmentSet m_attachments;

		private:
			static inline uint32_t sm_renderpass_count = 0;
		};

		class GraphicsPipeline
		{
			friend class GRI;
			friend class RenderPass; // Create in RenderPass
		protected:
			// Please Bind before Start!
			virtual void Begin(std::shared_ptr<CommandBuffer> commandbuffer) = 0; // We gave a sample in GRI.cc
			virtual void End(std::shared_ptr<CommandBuffer> commandbuffer)	 = 0; // We gave a sample in GRI.cc
			operator VkPipeline() const { return m_handle; }

		public:
			struct ShaderModule
			{
				std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
				std::shared_ptr<Shader> vertex_shader;
				std::shared_ptr<Shader> fragment_shader;	
			};
			GraphicsPipeline(ShaderModule shader_module);
			GraphicsPipeline(const char* signature); // Skip initialization
			virtual ~GraphicsPipeline() noexcept;
			GraphicsPipeline() = delete;

		protected:
			virtual auto vertex_input_state()	-> VkPipelineVertexInputStateCreateInfo;
			virtual auto tessellation_state()	-> VkPipelineTessellationStateCreateInfo;
			virtual auto input_assembly_state()	-> VkPipelineInputAssemblyStateCreateInfo;
			virtual auto viewport_state()		-> VkPipelineViewportStateCreateInfo;
			virtual auto rasterization_state()	-> VkPipelineRasterizationStateCreateInfo;
			virtual auto multisampling_state()	-> VkPipelineMultisampleStateCreateInfo;
			virtual auto depth_stencil_state()	-> VkPipelineDepthStencilStateCreateInfo;
			virtual auto color_blend_state()	-> VkPipelineColorBlendStateCreateInfo;
			virtual auto dynamic_state()		-> VkPipelineDynamicStateCreateInfo;

		protected:
			VkPipeline		 m_handle		  { VK_NULL_HANDLE };
			VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };
			ShaderModule						m_shader_module;
			VkViewport							m_viewport;
			VkRect2D							m_scissor;
			VkPipelineColorBlendAttachmentState m_color_blend;

		private:
			const char* m_signature = nullptr; // For Skipping Creation
		};

    private:
		// Global Command Pools
		using GRICommandPool = std::unordered_map<QueueFamilyType, std::shared_ptr<CommandPool>>;
		static inline std::unordered_map<std::thread::id, GRICommandPool> sm_normal_command_pools;
		static inline std::unordered_map<std::thread::id, GRICommandPool> sm_auto_free_command_pools;
		static inline std::unordered_map<std::thread::id, GRICommandPool> sm_auto_reset_command_pools;

		// Global Descriptor Objects
		static inline std::unordered_map<std::string, std::shared_ptr<DescriptorSetLayout>>sm_descriptor_set_layouts;
		static inline std::unordered_map<std::thread::id, std::shared_ptr<DescriptorPool>> sm_descriptor_pools;

		// Global Samplers
		static inline std::unordered_map<std::string, std::shared_ptr<Sampler>> sm_samplers;

	private:
		// Frame Tasks
		struct FrameTask
		{
			Fence fence{ FenceType_Unsignaled };
			std::vector<VkCommandBuffer> commandbuffers;
		};
		using FrameTaskMap = std::unordered_map<QueueFamilyType, FrameTask>;
		static inline std::unordered_map<std::thread::id, FrameTaskMap> sm_preframe_tasks;
		static void do_preframe_tasks();

	private:
		struct RenderTarget
		{
			static inline std::shared_ptr<Image> zbuffer; // Shared
			std::shared_ptr<Image> image;

			std::shared_ptr<CommandBuffer> commandbuffer;
			
			GRI::Fence fence_in_flight{ FenceType_Signaled };
			Semaphore  semaphore_ready{SemaphoreType_Unsignaled};
		};
		static void create_render_targets();
		static void destroy_render_targets();
		static inline std::vector<RenderTarget> sm_render_targets;

    private:
        GRI()                      = delete;
        GRI(const GRI&)            = delete;
        GRI(GRI&&)                 = delete;
        GRI& operator=(const GRI&) = delete;
        GRI& operator=(GRI&&)      = delete;
    };
} // namespace Albedo