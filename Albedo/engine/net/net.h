#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoNet.hpp>
#include <AlbedoLog.hpp>
#include <AlbedoTime.hpp>

#include "socket/socket.h"

#include "protocol/chamber.pb.h"

#include <core/math/math.h>

namespace Albedo {
namespace Net
{
	using namespace Albedo::Core;

	class Socket;

	class NetModule:
		public pattern::Singleton<NetModule>
	{
		friend class pattern::Singleton<NetModule>;
		NetModule();

	public:
		Matrix4f GetCameraView(uint32_t player_id);

	public:
		bool IsOnline() const { return m_socket->isConnected(); }
		void Reconnect();

	public:
		void Run(std::string host, const uint16_t port);

	private:
		std::unique_ptr<Socket> m_socket; // Client
		std::unique_ptr<net::HandlerPool> m_handler_pool;

		std::string	m_host;
		uint16_t		m_port = 0;

		struct NetDataPool // Visit via AlbedoNetData::Usage
		{
			using NetDataMap = std::unordered_map<Chamber::DataUsage, std::pair<std::mutex, AlbedoNetData>>;
			NetDataMap mutex_and_data;
		};
		std::vector<NetDataPool> m_netdata_pools; // 1 pool for 1 player

		std::condition_variable m_data_processor_notifier;
		std::mutex m_data_processor_mutex;
		std::thread m_data_processor;
	};

}} // namespace Albedo::Net