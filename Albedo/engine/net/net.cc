#include "net.h"

#include "socket/socket.h"

namespace Albedo {
namespace Net
{

	NetModule::NetModule():
		m_socket{ std::make_unique<Socket>() },
		m_handler_pool{ std::make_unique<net::HandlerPool >([](net::MID mID)->net::HID {return mID / 100; }) }
	{

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
			m_is_online = true;
		}
		catch (std::runtime_error& err) // Failed to connect
		{
			log::error("Albedo Net Module: {}", err.what());
		}
	}

}} // namespace Albedo::Net