#include <runtime/function_layer/UI/UI_system.h>

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
				{
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

					ImGui::SeparatorText("Chamber");
					static std::string displayName;
					displayName.reserve(64);
					for (const auto& [id, player] : m_chamber_players)
					{
						displayName = std::format("{} ({})", player.profile.nickname(), player.profile.uid());
						ImGui::RadioButton(displayName.c_str(), true);
					}
				}
				ImGui::End();
			});
	}
	
	void NetModule::Run(std::string_view uid, std::string_view nickname, std::string_view host, std::string_view port, std::string_view pass)
	{
		static bool RUNNING = false;
		if (RUNNING) log::warn("You tried to run Net Module again!");
		RUNNING = true;

		log::debug("Net Module Run:  {}({}) Host: {} Port: {}", nickname, uid, host, port);
		try
		{
			m_host = host;
			m_port = atoi(port.data());
			m_pass = pass;

			m_socket->connect(host, m_port);
	
			int _time_limit = 50; // 5s
			while (!IsOnline() && _time_limit--) std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!IsOnline()) throw std::runtime_error("Failed to connect to server!");
			else log::debug("Connected to server successfully!");

			m_profile.set_nickname(nickname.data());
			m_profile.set_uid(atoi(uid.data()));
			m_profile.set_pass(m_pass);

			m_socket->sendToServer(net::Message{
				ABDChamber::DataUsage::CHAMBER_LOGIN,
				m_profile.SerializeAsString() });

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
							//if (!envelope.sender()) log::error("NO SENDER!! -- EXPERIED");
							//else
							{
								auto& header = envelope.message().header;
								auto& body = envelope.message().body;
	
								switch (header.message_id)
								{
									case ABDChamber::DataUsage::camera_matrics:
									{
										ABDChamber::ChamberData cb_data;
										if (cb_data.ParseFromString(body.message))
										{
											std::scoped_lock guard{ m_chamber_mutexes[cb_data.uid()] };

											auto& player = m_chamber_players[cb_data.uid()];
											auto& bufferView_viewmat = cb_data.buffer().views().Get(0);
											const float* viewmat = cb_data.buffer().content().data() + bufferView_viewmat.offset();
											
											if (viewmat) player.camera_view_matrix = Matrix4f(viewmat);
											else log::warn("Got empty camera matrics");
										}
										else log::warn("Failed to parse ChamberData protobuf");
											
										break;
									}
									case ABDChamber::DataUsage::CHAMBER_LOGIN:
									{
										ABDChamber::ChamberLogin profile;
										if (profile.ParseFromString(body.message))
										{
											if (m_chamber_players.find(profile.uid()) == m_chamber_players.end())
											{
												log::info("A new member {} joins in chamber.", profile.nickname());
												m_chamber_players[profile.uid()].camera_view_matrix.setZero();
												m_chamber_players[profile.uid()].profile = std::move(profile);
											}
										}
										else log::warn("Failed to parse ChamberLogin protobuf");

										break;
									}
									case ABDChamber::DataUsage::chamber_message:
									{
										log::info("[Chamber]: {}", body.message);
										break;
									}
									default: log::warn("Unknown Chamber Data Usage!");
								}
							}
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

	void NetModule::SyncCamera(ABDChamber::Buffer* camera_data)
	{
		if (!IsOnline()) return;

		ABDChamber::ChamberData chamber_data;
		
		chamber_data.set_uid(m_profile.uid());
		chamber_data.set_usage(ABDChamber::DataUsage::camera_matrics);

		chamber_data.set_allocated_buffer(camera_data);

		static std::string serializedData;
		chamber_data.SerializeToString(&serializedData);

		m_socket->sendToServer(net::Message{ 
			ABDChamber::DataUsage::camera_matrics, serializedData });

	}

}} // namespace Albedo::Net