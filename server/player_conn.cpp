#include "player_conn.hpp"

namespace autocraft {

player_conn::player_conn(connection&& parent)
    : connection(std::move(parent))
{}

void player_conn::interpret(std::string&& message)
{

}

player_conn::patch::patch(std::string&& w, any&& v)
    : what(std::move(w))
    , value(std::move(v))
{}

std::string const& player_conn::player_name() const
{
    return player_name_;
}

void player_conn::finish_auth()
{
    nlohmann::json responseJSON;
    responseJSON["authenticated"] = true;
    responseJSON["spawnPosition"]["x"] = 1;
    responseJSON["spawnPosition"]["y"] = 1;
    responseJSON["spawnPosition"]["z"] = 1;
    AUTOCRAFT_CONN_LOG("ABOUT TO WRITE: " << responseJSON.dump());
    write(std::make_shared<std::string>(std::move(responseJSON.dump())));
}

//void player_conn::on_player_load(std::shared_ptr<player> const& player_entity)
//{
//    if (state_ != loading_player)
//        return;

//    CONN_LOG("PLAYER LOADED id=" << player_entity->id());

//    player_entity_ = player_entity;
//    player_entity->set_conn(this);

//    server_->register_player(shared_from_this(), player_entity);

//    state_ = reading;
//}

// player_conn::patch player_conn::pop_patch()
// {
//     AUTOCRAFT_LOCK(patches_mutex_);

//     patch p = std::move(patches_.front());
//     patches_.pop();
//     return p;
// }

// bool player_conn::has_patch() const
// {
//     AUTOCRAFT_LOCK(patches_mutex_);

//     return !patches_.empty();
// }

// void player_conn::push_patch(std::string && what, any && value)
// {
//     AUTOCRAFT_LOCK(patches_mutex_);

//     push_patch(patch(std::move(what), std::move(value)));
// }

// void player_conn::push_patch(patch && p)
// {
//     AUTOCRAFT_LOCK(patches_mutex_);

//     patches_.emplace(std::move(p));
// }

} // namespace autocraft
