#include "present.h"

#include <AlbedoCore/Log/log.h>
#include <AlbedoSystem/UI/UI_system.h>

#include "../../../../editor/editor.h"

namespace Albedo{
namespace APP
{
	
	EditorPipeline::
	EditorPipeline():
		GRI::GraphicsPipeline("Surface::Editor")
	{

	}

	void
	EditorPipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording() && "You cannot Begin() before beginning the command buffer!");
		Editor::Tick(commandbuffer);
	}

	void
	EditorPipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording() && "You cannot End() before beginning the command buffer!");
		// You may need to call vkCmdNextSubpass(...);
	}

}} // namespace Albedo::APP