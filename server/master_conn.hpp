#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>

#include "connection.hpp"
#include "log.hpp"

#define MASTER_LOG(to_log) AUTOCRAFT_LOG("<MASTER>addr_str", to_log)

namespace autocraft
{
	class master_conn: public connection
	{
	public:

	    master_conn(connection &&parent)
		: connection(std::move(parent))
		{}
	    
		void start()
		{
			MASTER_LOG("STARTED");
		}

		virtual void finish_auth()
		{
			nlohmann::json responseJSON;
			responseJSON["authenticated"] = true;
			responseJSON["spawnPosition"]["x"] = 0;
			responseJSON["spawnPosition"]["y"] = 0;
			responseJSON["spawnPosition"]["z"] = 0;
			AUTOCRAFT_CONN_LOG("ABOUT TO WRITE: " << responseJSON.dump());
			write(std::make_shared<std::string>(std::move(responseJSON.dump())));
		}
	};
}
