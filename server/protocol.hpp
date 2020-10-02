#pragma once

#include <memory>
#include <string>

#include "config.hpp"
#include "entities.hpp"

namespace autocraft {

class player;

AUTOCRAFT_API extern std::string json_state_entities(entities const& entities);
AUTOCRAFT_API extern std::string json_state_player(std::shared_ptr<player const> e);

} // namespace autocraft
