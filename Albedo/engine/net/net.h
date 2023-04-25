#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoNet.hpp>
#include <AlbedoLog.hpp>

#include "socket/socket.h"


namespace Albedo {
namespace Net
{
	class Socket;

	class NetModule:
		public pattern::Singleton<NetModule>
	{
		friend class pattern::Singleton<NetModule>;
		NetModule();

	public:
		//bool Reconnect();

	public:
		void Run(std::string host, const uint16_t port);

	private:
		std::unique_ptr<Socket> m_socket; // Client
		std::unique_ptr<net::HandlerPool> m_handler_pool;

		std::string	m_host;
		uint16_t		m_port;
		bool				m_is_online = false;
	};

}} // namespace Albedo::Net