#include "UI.h"
#include <AlbedoCore/Log/log.h>
#include <AlbedoSystem/UI/UI_system.h>

namespace Albedo{
namespace APP
{
	
	UIPipeline::
	UIPipeline():
		GRI::GraphicsPipeline("Surface::UI")
	{

	}

	void
	UIPipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording() && "You cannot Begin() before beginning the command buffer!");
		UISystem::Process(commandbuffer);
	}

	void
	UIPipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording() && "You cannot End() before beginning the command buffer!");
		// You may need to call vkCmdNextSubpass(...);
	}

}} // namespace Albedo::APP