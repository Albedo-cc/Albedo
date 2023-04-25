#include <runtime/function_layer/UI/UI_system.h>
#include <net/protocol/data_layout.pb.h>

#include "net.h"

namespace Albedo {
namespace Net
{

	NetModule::NetModule():
		m_socket{ std::make_unique<Socket>() },
		m_handler_pool{ std::make_unique<net::HandlerPool >([](net::MID mID)->net::HID {return mID / 100; }) }
	{
		Runtime::UISystem::instance().RegisterUIEvent(
			"Albedo Net", [this]()
			{
				static time::StopWatch<float> timer;

				ImGui::Begin("Net State");

				bool is_online = IsOnline();
				ImGui::RadioButton("Online", is_online);
				ImGui::SameLine();

				static int connect_wait_time = 0; // 5 secs
				static bool should_disable = false;
				static int delta_time = 0;
				if (connect_wait_time)
				{
					delta_time = timer.split().seconds() - connect_wait_time;
					if (delta_time >= 5) connect_wait_time = 0;
				}
				should_disable = is_online || connect_wait_time;
				if (should_disable) ImGui::BeginDisabled();
				if (ImGui::Button("Connect"))
				{
					Reconnect();
					connect_wait_time = timer.split().seconds();
				}
				if (should_disable) ImGui::EndDisabled();

				ImGui::End();
			});
	}
	
	void NetModule::Run(std::string host, const uint16_t port)
	{
		static bool RUNNING = false;
		if (RUNNING) log::warn("You tried to run Net Module again!");
		RUNNING = true;

		try
		{
			m_socket->connect(host, port);
			m_host = std::move(host);
			m_port = port;
			m_data_processor = std::thread([this]() 
				{
					while (true)
					{
						auto& mailbox = m_socket->getMessageDeque();
						mailbox.wait();

						while (!mailbox.empty())
						{
							auto envelope = mailbox.pop_front();
							auto& msg = envelope.message();
							if (msg.intact())
							{
								AlbedoNetData::Data netdata;
								netdata.ParseFromString(msg.body.message);
								
							}
							else log::warn("Discarded a non-intact message!");
						}
					}
				});
			m_data_processor.detach(); // Daemon Thread
		}
		catch (std::runtime_error& err) // Failed to connect
		{
			log::error("Albedo Net Module: {}", err.what());
		}
	}

	void NetModule::Reconnect()
	{
		try
		{
			m_socket->connect(m_host, m_port, true);
		}
		catch (std::runtime_error& err) // Failed to connect
		{
			log::error("Albedo Net Module: {}", err.what());
		}
	}

	Matrix4f NetModule::GetCameraView(uint32_t player_id)
	{
		uint32_t token = AlbedoNetData::Usage::camera_view_matrix;
		if (player_id < m_netdata_pools.size())
		{
			std::scoped_lock guard{ m_netdata_pools[player_id].mutexes[token]};
			return Matrix4f(m_netdata_pools[player_id].data.data());
		}
		else log::warn("Failed to get camera view of player {} - Out of range!", player_id);
	}

}} // namespace Albedo::Net