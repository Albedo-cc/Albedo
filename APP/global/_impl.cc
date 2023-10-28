#include "config.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.File>

namespace Albedo{
namespace APP
{

	APPConfig::
	APPConfig()
	{
		JSONFile config{ "config/app.json" };
		m_parameters = std::move(Parameters
		{
			.app
			{
				.name = config["Name"],
			},
			.window
			{
				.title = config["Window"]["Title"],
				.icon  = config["Window"]["Icon"],
				.width = config["Window"]["Extent"][0],
				.height= config["Window"]["Extent"][1],
				.options
				{
					.maximize  = config["Window"]["Options"]["Maximize"],
					.resizable = config["Window"]["Options"]["Resizable"],
				}
			},
			.editor
			{
				.layout = config["Editor"]["Layout"],
				.font
				{
					.name = config["Editor"]["Font"][0],
					.size = config["Editor"]["Font"][1],
				},
				.options
				{
					.save_layout = config["Editor"]["Options"]["SaveLayout"],
				}
			},
		});
	}

	APPConfig::
	~APPConfig() noexcept
	{
		Log::Info("Saving APP configurations");
		// WIP
	}

}} // namespace Albedo::APP