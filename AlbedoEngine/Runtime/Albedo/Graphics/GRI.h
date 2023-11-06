#pragma once
/**
 * @file       GRI.h
 * @brief      GRI(Graphics Render Interface), User-level interface.
 * @author     LJYC(https://github.com/LJYC-ME)
 */

// Header Mess
#include "GRIEnums.h"

#include <array>
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
typedef struct GLFWwindow			GLFWwindow;
typedef struct VmaAllocator_T*		VmaAllocator;
typedef struct VmaAllocation_T*		VmaAllocation;
typedef struct ktxVulkanDeviceInfo	ktxVulkanDeviceInfo;

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
	/*Memory*/			class Shader; class Buffer;
	/*Interface Class*/	class RenderPass; class Pipeline; class GraphicsPipeline;
	/*Descriptor*/		class DescriptorSetLayout; class DescriptorPool; class DescriptorSet;
	/*Texture*/			class Texture; class Texture2D; class Cubemap;
	/*Sampler*/			class Sampler;
	//[TIPS]-----------------------------------------------------------------------------------------------------------------------
	// 1. For better Interface Compatibility, you should create GRI objects via [Class]::Create(...).
	//    Importantly, DO NOT create GRI objects without [Class]::Create(...) by yourself!
	//-----------------------------------------------------------------------------------------------------------------------------
	
	public: // User-level Interfaces (e.g. Manage Global GPU Resource)
		static void RegisterGlobalDescriptorSetLayout(std::string id, std::shared_ptr<DescriptorSetLayout> descriptor_set_layout);
		static void RegisterGlobalSampler(std::string id, std::shared_ptr<Sampler> sampler);
		static void RegisterGlobalTexture(std::string id, std::shared_ptr<Texture> texture);

		static auto GetGlobalDescriptorSetLayout(const std::string& id) -> std::shared_ptr<GRI::DescriptorSetLayout>;
		static auto GetGlobalDescriptorPool(std::thread::id thread_id = std::this_thread::get_id()) -> std::shared_ptr<GRI::DescriptorPool>;
		static auto GetGlobalCommandPool(CommandPoolType type, QueueFamilyType queue, std::thread::id thread_id = std::this_thread::get_id()) -> std::shared_ptr<GRI::CommandPool>;
		static auto GetGlobalSampler(const std::string& id) -> std::shared_ptr<Sampler>;
		static auto GetGlobalTexture(const std::string& id) -> std::shared_ptr<Texture>;

		static auto GetQueue(QueueFamilyType queue_family, uint32_t index = 0) -> VkQueue;
		static auto GetQueueFamilyIndex(QueueFamilyType queue_family) -> uint32_t;
		static auto GetCurrentRenderTarget() -> std::shared_ptr<const GRI::Texture>;
		static auto GetZBuffer()			 -> std::shared_ptr<const GRI::Texture>;
		static auto GetRenderTargetCount()	 -> size_t;
		static auto GetRenderTargetCursor()	 -> uint32_t;
		static auto GetRenderTargetFormat()	 -> VkFormat;

		static void PushPreframeTask(std::shared_ptr<CommandBuffer> commandbuffer); //[WARN]: will be depreciated!

		static void Screenshot(std::shared_ptr<Texture> output);

		static auto MakeID(const std::vector<VkDescriptorType>& types_in_order) -> std::string;
		static auto PadUniformBufferSize(size_t original_size) -> size_t;

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
		static void recreate_swapchain();

	public: // GRI Objects
        class Fence final
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
				std::vector<VkSemaphore>		  wait_semaphores;
				std::vector<VkSemaphore>		  signal_semaphores;
			};

		public:
			virtual void Begin();
			virtual void End();
			virtual void Submit(const SubmitInfo& submitinfo, VkFence signal_fence = VK_NULL_HANDLE);
			auto IsRecording() const -> bool { return m_is_recording; }
			auto GetSettings() const -> const CreateInfo& { return m_settings; }
			auto GetSubpassContents() const -> VkSubpassContents {return m_settings.level == VK_COMMAND_BUFFER_LEVEL_PRIMARY ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;}
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
			void SubmitCommandBuffers(const std::vector<VkSubmitInfo>& submitinfos, VkFence signal_fence = VK_NULL_HANDLE);
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
		{
			friend class GRI;
		public:
			auto GetBinding(uint32_t index) const -> const VkDescriptorSetLayoutBinding&;
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
		{
			friend class GRI;
		public:
			// GRI::UpdateDescriptorSets({SetA.WriteBuffer(...), SetB.WriteImage(...), ...}
			auto BindToBuffer(uint32_t binding, std::shared_ptr<Buffer> buffer, size_t offset, size_t size) -> VkWriteDescriptorSet;
			auto BindToTexture(uint32_t binding, std::shared_ptr<Texture> texture)  -> VkWriteDescriptorSet;

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
			void WriteAll(void* data); // The buffer must be mapping-allowed and writable
			void Write(void* data, size_t size, size_t offset);
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

		
		class Sampler final
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
		{
			friend class GRI;
		public:
			struct CreateInfo // Create by GRI
			{
				VkImageAspectFlags		 aspect;
				VkImageUsageFlags		 usage;
				VkFormat				 format;
				VkExtent3D				 extent;
				VkImageViewType			 viewType	= VK_IMAGE_VIEW_TYPE_MAX_ENUM;//Auto
				uint32_t				 mipLevels	= 1;
				uint32_t				 arrayLayers= 1;
				VkSampleCountFlagBits    samples	= VK_SAMPLE_COUNT_1_BIT;
				VkImageTiling			 tiling		= VK_IMAGE_TILING_OPTIMAL;
			};

		public:
			void Write(std::shared_ptr<CommandBuffer> commandbuffer, std::shared_ptr<Buffer> data);
			void Blit(std::shared_ptr<CommandBuffer> commandbuffer, std::shared_ptr<Texture>  target) const;
			void Resize(std::shared_ptr<CommandBuffer> commandbuffer, const VkExtent3D& extent);
			void ConvertLayout(std::shared_ptr<CommandBuffer> commandbuffer, VkImageLayout target_layout);

			auto GetSettings()			const -> const CreateInfo& { return m_settings; }
			auto GetLayout()			const -> VkImageLayout{ return m_layout; }
			auto GetView()				const -> VkImageView { return m_view; }
			auto GetSampler()			const -> std::shared_ptr<Sampler> { return m_sampler; }
			auto GetFormat()			const -> VkFormat { return m_settings.format; }
			auto GetExtent()			const -> const VkExtent3D& { return m_settings.extent; }
			auto GetMipmapLevels()		const -> uint32_t { return m_settings.mipLevels; }
			auto GetArraryLevels()		const -> uint32_t { return m_settings.arrayLayers; }
			auto GetSubresourceRange()	const ->VkImageSubresourceRange;
			auto GetSubresourceLayers() const ->VkImageSubresourceLayers;
			auto GetSize()				const ->VkDeviceSize;
			operator VkImage()			const { return m_handle; }

			auto HasStencil()			const -> bool;
			auto HasMipmap()			const -> bool { return m_settings.mipLevels - 1; }

		public:
			static inline auto Create(GRI::Texture::CreateInfo createinfo, std::shared_ptr<Sampler> sampler = nullptr/*Auto*/)
			{ return std::make_shared<GRI::Texture>(createinfo, sampler); }
			Texture(CreateInfo createinfo, std::shared_ptr<Sampler> sampler = nullptr);
			Texture(std::string_view ktx_texture_path, std::shared_ptr<Sampler> sampler = nullptr);
			virtual ~Texture() noexcept;

		protected:
			void fill_convert_layout_info(VkImageMemoryBarrier& barrier, 
				VkImageLayout source_layout, VkImageLayout target_layout, 
				VkPipelineStageFlags& from_stage, VkPipelineStageFlags& to_stage) const;

		protected:
			VkImage			m_handle		{ VK_NULL_HANDLE };
			VkImageView		m_view			{ VK_NULL_HANDLE };
			VkImageLayout	m_layout		{ VK_IMAGE_LAYOUT_UNDEFINED };
			CreateInfo		m_settings;
			VmaAllocation	m_allocation	{ VK_NULL_HANDLE };
			std::shared_ptr<Sampler> m_sampler;
			//[INTRODUCTION]
			//[Combine KTX with VMA](https://github.com/KhronosGroup/KTX-Software/pull/748)
		};

		class Texture2D
			:public Texture
		{
			friend class GRI;
		public:
			void Resize(std::shared_ptr<CommandBuffer> commandbuffer, const VkExtent2D& extent);

		public:
			static inline auto Create(GRI::Texture::CreateInfo createinfo, std::shared_ptr<Sampler> sampler = nullptr)
			{ return std::make_shared<Texture2D>(std::move(createinfo), std::move(sampler)); }
			Texture2D(GRI::Texture::CreateInfo createinfo, std::shared_ptr<Sampler> sampler = nullptr);
			Texture2D() = delete;
		};

		
		class Cubemap
			:public Texture
		{
			friend class GRI;
		public:
			// Vulkan Cubemap uses left-hand coordinate system with +Y is up. (as same as Albedo Coordinate System)
			enum Face
			{PositiveX, NegtiveX, PositiveY, NegtiveY, PositiveZ, NegtiveZ, MAX_FACE_NUM};
			static constexpr uint32_t VertexCount = 24; // Ref::cubemap.vert
		public:
			//void WriteFace(std::shared_ptr<CommandBuffer> commandbuffer, Face face, std::shared_ptr<Buffer> data);

		public:
			/*static inline auto Create(std::shared_ptr<Texture> image, std::shared_ptr<Sampler> sampler = nullptr)
			{ return std::make_shared<GRI::Cubemap>(image, sampler); }*/
			//Cubemap(std::shared_ptr<Texture> image, std::shared_ptr<Sampler> sampler = nullptr);
			virtual ~Cubemap() noexcept override;
		};

		class RenderPass
		{
			friend class GRI;
		public:
			struct SubpassSetting;
			class SubpassIterator
			{
				friend class RenderPass;
			public:
				auto GetName() const  -> std::string_view { return iter_begin->name; }
				void Begin() const { iter_begin->pipeline->Begin(commandbuffer); }
				void End() const { iter_begin->pipeline->End(commandbuffer); }
				bool Next() { if (++iter_begin != iter_end){ vkCmdNextSubpass(*commandbuffer, commandbuffer->GetSubpassContents()); return true;} return false; }

			private:
				SubpassIterator(std::shared_ptr<CommandBuffer> commandbuffer,
								std::vector<SubpassSetting>::iterator subpass_begin,
								std::vector<SubpassSetting>::iterator subpass_end) :
								commandbuffer{ std::move(commandbuffer) },
								iter_begin{ subpass_begin }, iter_end{subpass_end} {}
				std::shared_ptr<CommandBuffer> commandbuffer;
				std::vector<SubpassSetting>::iterator iter_begin;
				std::vector<SubpassSetting>::iterator iter_end;
			};

		public:
			virtual auto Begin(std::shared_ptr<CommandBuffer> commandbuffer) -> SubpassIterator;
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
				VkClearValue			clearColor{{1,0,1,1}}; // Purple(Color) | One(ZBuffer)
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
			enum : uint32_t { ClearSTColor = 1 << 0, ZWrite = 1 << 1 };
			void BEGIN_BUILD(uint32_t flags = 0);
			auto add_attachment(AttachmentSetting setting) -> uint32_t;
			auto add_subpass(SubpassSetting setting)	   -> uint32_t;
			void END_BUILD();

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
				std::vector<VkAttachmentDescription> descriptions;
				std::vector<VkClearValue>			 clear_colors;
			};
			AttachmentSet m_attachments;

		private:
			static inline uint32_t sm_renderpass_count = 0;
		};

		class Pipeline
		{
			friend class GRI;
		protected:
			virtual void Begin(std::shared_ptr<CommandBuffer> commandbuffer);
			virtual void End(std::shared_ptr<CommandBuffer> commandbuffer)	;
			operator VkPipeline() const { return m_handle; }

		public:
			Pipeline() = default;
			virtual ~Pipeline() noexcept;

		protected:
			VkPipeline		 m_handle		  { VK_NULL_HANDLE };
			VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };
		};

		class GraphicsPipeline:
			public Pipeline
		{
			friend class GRI;
			friend class RenderPass; // Create in RenderPass
		public:
			struct ShaderModule
			{
				std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
				std::shared_ptr<Shader> vertex_shader;
				std::shared_ptr<Shader> fragment_shader;	
			};
			GraphicsPipeline(ShaderModule shader_module);
			GraphicsPipeline();
			virtual ~GraphicsPipeline() noexcept;

		protected:
			// Set return type as static for flyweight.
			virtual auto vertex_input_state()	-> const VkPipelineVertexInputStateCreateInfo&;
			virtual auto tessellation_state()	-> const VkPipelineTessellationStateCreateInfo&;
			virtual auto input_assembly_state()	-> const VkPipelineInputAssemblyStateCreateInfo&;
			virtual auto viewport_state()		-> const VkPipelineViewportStateCreateInfo&;
			virtual auto rasterization_state()	-> const VkPipelineRasterizationStateCreateInfo&;
			virtual auto multisampling_state()	-> const VkPipelineMultisampleStateCreateInfo&;
			virtual auto depth_stencil_state()	-> const VkPipelineDepthStencilStateCreateInfo&;
			virtual auto color_blend_state()	-> const VkPipelineColorBlendStateCreateInfo&;
			virtual auto dynamic_state()		-> const VkPipelineDynamicStateCreateInfo&;

		protected:
			ShaderModule						m_shader_module;
			VkViewport							m_viewport;
			VkRect2D							m_scissor;
			VkPipelineColorBlendAttachmentState m_color_blend;
		};

	private:
		class VMA final
		{
			friend class GRI;
			VmaAllocator handle{ VK_NULL_HANDLE };
			operator VmaAllocator() const { return handle; }

			void Create();
			void Destroy();
		};
		static inline VMA sm_vma;

		class KTX final
		{
			friend class GRI;
			ktxVulkanDeviceInfo* context;
			operator ktxVulkanDeviceInfo*() const { return context; }

			void Create();
			void Destroy();
		};
		static inline KTX sm_ktx;

    private:
		// Global Command Pools
		using GRICommandPool = std::unordered_map<QueueFamilyType, std::shared_ptr<CommandPool>>;
		static inline std::unordered_map<std::thread::id, GRICommandPool> sm_normal_command_pools;
		static inline std::unordered_map<std::thread::id, GRICommandPool> sm_auto_free_command_pools;
		static inline std::unordered_map<std::thread::id, GRICommandPool> sm_auto_reset_command_pools;

		// Global Descriptor Objects
		static inline std::unordered_map<std::string, std::shared_ptr<DescriptorSetLayout>>sm_descriptor_set_layouts;
		static inline std::unordered_map<std::thread::id, std::shared_ptr<DescriptorPool>> sm_descriptor_pools;

		// Global Resource
		static inline std::unordered_map<std::string, std::shared_ptr<Sampler>> sm_global_samplers;
		static inline std::unordered_map<std::string, std::shared_ptr<Texture>> sm_global_textures;

	private:
		// Frame Tasks
		struct FrameTask
		{
			Fence fence{ FenceType_Unsignaled };
			std::unordered_map<VkCommandPool, std::vector<VkCommandBuffer>> task_set;
		};
		static inline std::vector<FrameTask> sm_preframe_task_pools;
		static void do_preframe_tasks(); //[TODO]: Delete, instead wating Transfer Queue before a new frame.

	private:
		struct RenderTarget
		{
			static inline std::shared_ptr<Texture> zbuffer; // Shared
			std::shared_ptr<Texture> image;
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