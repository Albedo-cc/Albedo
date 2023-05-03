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
		using UID = int32_t; // UID
		using PlayerInfo = ABDChamber::ChamberLogin;
		struct PlayerState
		{
			PlayerInfo profile;

			Matrix4f camera_view_matrix;
		};
		const std::unordered_map<UID, PlayerState>& ViewPlayers() const
		{return m_chamber_players;}
		PlayerState GetPlayerState(UID player_id)
		{
			std::scoped_lock guard{ m_chamber_mutexes[player_id] };
			return m_chamber_players[player_id];
		}

	private:
		ABDChamber::ChamberLogin m_profile;
		std::unordered_map<UID, std::mutex> m_chamber_mutexes;
		std::unordered_map<UID, PlayerState> m_chamber_players;

	public:
		void SyncCamera(ABDChamber::Buffer* camera_data);
		void SyncMessage(std::string_view message);
		std::vector<std::string> console_messages; // [FIXME]: Need Reconstruction!

	public:
		bool IsOnline() const { return m_socket->isConnected(); }
		void Reconnect();

	public:
		void Run(std::string_view uid, std::string_view nickname, std::string_view host, std::string_view port, std::string_view pass);

	private:
		std::unique_ptr<Socket> m_socket; // Client
		std::unique_ptr<net::HandlerPool> m_handler_pool;

		std::string	m_host;
		uint16_t		m_port = 0;
		std::string  m_pass;

		std::thread m_data_processor;
	};

}} // namespace Albedo::Net