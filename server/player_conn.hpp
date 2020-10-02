#pragma once

#pragma once

//#include <boost/asio/ip/tcp.hpp>
//#include <boost/beast/websocket/stream.hpp>

#include "connection.hpp"

namespace autocraft
{
	class player_conn : public connection
	{
	public:
		struct patch
		{
			std::string what;
			any         value;

			patch(std::string&& w, any&& v);
			patch(patch&& other) = default;
			patch& operator=(patch&&) = default;

			patch(patch const&) = delete;
			patch& operator=(patch const&) = delete;

		};
	private:
		std::shared_ptr<player>                                           player_entity_;
		std::string                                                       player_name_;
		//std::queue<patch>                                                 patches_;

	public:
		player_conn(connection&& parent);

	public:
		std::shared_ptr<player> const& player_entity() const;
		bool                            is_ready() const;
		std::string const& player_name() const;

		virtual void interpret(std::string&& message);
		virtual void finish_auth();
		//void on_player_load(std::shared_ptr<player> const& player_entity);

		/*patch                           pop_patch();
		bool                            has_patch() const;*/
		/*void push_patch(std::string && what, any && value);
		void push_patch(patch && p);*/
	};
}
