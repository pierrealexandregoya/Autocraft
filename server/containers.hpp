#pragma once

#include <memory>
#include <list>

namespace autocraft {

class connection;
class player_conn;

typedef std::list<std::shared_ptr<connection>> connections;
typedef std::list<std::shared_ptr<player_conn>> player_conns;

} // namespace autocraft
