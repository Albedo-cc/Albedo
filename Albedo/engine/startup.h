#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoTime.hpp>
#include <AlbedoLog.hpp>

#include "runtime/runtime.h"
#include "net/net.h"

#include <fstream>
#include <json.hpp>

namespace Albedo
{
	class APP :
		public pattern::Singleton<APP>
	{
		friend class pattern::Singleton<APP>;

		APP()
		{
			log::info("Albedo is being initialized");
			m_stopwatch.start();
		}

		~APP()
		{
			log::info("Albedo is being terminated, total run time {} s", m_stopwatch.split().seconds());
		}

	public:
		void Run(int argc, char* argv[])
		{
			load_configurations("config.json");

			if (config_online_mode)
			{
				Net::NetModule::instance().
					Run(config_online_uid, 
							config_online_name, 
							config_online_ip, 
							config_online_port, 
							config_online_pass);
			}
			Runtime::RuntimeModule::instance().Run();
		}

	private:
		time::StopWatch<uint64_t> m_stopwatch;
		// Configurations
		bool config_online_mode = false;
		std::string config_online_ip;
		std::string config_online_port;
		std::string config_online_pass;
		std::string config_online_uid;  
		std::string config_online_name;

	private:
		void load_configurations(const char* config_file)
		{
			using json = nlohmann::json;

			std::ifstream file("config.json");
			if (file.is_open())
			{
				json config = json::parse(file);
				auto config_online = config.find("Online");

				auto fetch_json_value = [&config](const char* key)->std::string
				{
					auto target = config.find(key);
					if (target == config.end())
					{
						log::error("Failed to find {} configuration item!", key);
						return "NOT FOUND";
					}
					return *target;
				};

				config_online_mode = (fetch_json_value("Online") == "1");
				if (config_online_mode)
				{
					log::info("APP starts in online mode");
					config_online_ip		= fetch_json_value("Online_IP");
					config_online_port	= fetch_json_value("Online_Port");
					config_online_uid		= fetch_json_value("Online_UID");
					config_online_name	= fetch_json_value("Online_Name");
					config_online_pass	= fetch_json_value("Online_Pass");
				}
				file.close();

				if (config_online_mode)
				{
					std::ofstream saveconfig{ config_file };
					if (saveconfig.is_open())
					{
						config["Online"] = "0";

						saveconfig << std::setw(4) << config << std::endl;
						saveconfig.close();
					}
					else throw std::runtime_error("Failed to save configuration file!");
				}
			}
			else throw std::runtime_error("Failed to open configuration file!");
		}
	};
}