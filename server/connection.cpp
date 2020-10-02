#include "player_conn.hpp"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

#include <nlohmann/json.hpp>

#include "any.hpp"
#include "lock.hpp"
#include "log.hpp"
#include "player.hpp"
#include "protocol.hpp"
#include "server.hpp"
#include "utils.hpp"
#include "vector.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;

namespace autocraft {

class entity;

std::vector<std::string> const connection::state_str = {
    "none",
    "ready",
    "handshaking",
    "authenticating",
    "loading_player",
    "reading",
    "writing",
    "to_be_closed",
    "closing",
    "closed"
};

connection::connection(asio::ip::tcp::socket&& socket, boost::asio::io_context &ioc, std::shared_ptr<server> const& server)
    : addr_str(socket.remote_endpoint().address().to_string() + ":" + std::to_string(socket.remote_endpoint().port()))
    , socket_(std::move(socket))
    , strand_(ioc)
    , state_(state::none)
     , close_code_(beast::websocket::close_code::none)
    , server_(server)
    , close_timer_(socket_.get_executor())
{
    state_ = state::ready;
    socket_.auto_fragment(true);

    AUTOCRAFT_CONN_LOG("CONNECTED");
}

connection::connection(connection&& other)
	: addr_str(std::move(other.addr_str))
	, socket_(std::move(other.socket_))
	, strand_(std::move(other.strand_))
	, state_(std::move(other.state_))
    , read_buffer_(std::move(other.read_buffer_))
    , to_write_(std::move(other.to_write_))
	, close_code_(std::move(other.close_code_))
	, server_(std::move(other.server_))
	, close_timer_(std::move(other.close_timer_))
{
    *const_cast<std::string*>(&other.addr_str) += "<moved>";
}
    
connection::~connection()
{
    AUTOCRAFT_CONN_LOG("DESTROYED");
}

void connection::start()
{
    AUTOCRAFT_LOCK(handlers_mutex_);

     socket_.async_accept(asio::bind_executor(strand_, std::bind(&connection::on_accept, shared_from_this(), std::placeholders::_1)));
     state_ = state::handshaking;

    //state_ = authenticating;

    //socket_.async_read_some(asio::buffer(buffer_, buffer_size_), asio::bind_executor(strand_, std::bind(&player_conn::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
    // socket_.async_read(read_buffer_, asio::bind_executor(strand_, std::bind(&player_conn::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
}

 void connection::write(std::shared_ptr<std::string const> msg)
 {
     AUTOCRAFT_LOCK(handlers_mutex_);

     to_write_.emplace_back(msg);

     if (to_write_.size() == 1)
         asio::post(socket_.get_executor(), asio::bind_executor(strand_, std::bind(&player_conn::write_next, shared_from_this())));
 }

 void connection::close()
 {
     /*if (socket_.get_executor().running_in_this_thread())
         do_close(beast::websocket::close_code::normal);
     else
         asio::post(socket_.get_executor(), asio::bind_executor(strand_, std::bind(&player_conn::do_close, shared_from_this(), beast::websocket::close_code::normal)));*/
     socket_.close(beast::websocket::close_code::normal);
     state_ = state::closed;
 }

bool connection::is_closed() const
{
    AUTOCRAFT_LOCK(handlers_mutex_);
    return socket_.is_open() == false;
    //return state_ == closed;
}

std::shared_ptr<player> const& player_conn::player_entity() const
{
    return player_entity_;
}

connection::state connection::current_state() const
{
    return state_;
}

bool player_conn::is_ready() const
{
    return state_ == state::reading || state_ == state::writing;
}

 void connection::write_next()
 {
     AUTOCRAFT_LOCK(handlers_mutex_);

     if (/*!(state_ == loading_player || state_ == reading) || */to_write_.empty())
         return;

     //beast::async_write(socket_, asio::buffer(to_write_.front()->data(), to_write_.front()->size()), asio::bind_executor(strand_, std::bind(&player_conn::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
     socket_.async_write(asio::buffer(*to_write_.front()), asio::bind_executor(strand_, std::bind(&player_conn::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
     //socket_.async_write_some()
     //socket_.async_write(asio::buffer(*to_write_.front()), asio::bind_executor(strand_, std::bind(&player_conn::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
     state_ = state::writing;
 }

 void connection::on_accept(boost::system::error_code const& ec) noexcept
 {
     AUTOCRAFT_LOCK(handlers_mutex_);

     if (ec)
     {
         AUTOCRAFT_CONN_LOG("HANDSHAKE ERROR" << ": " << ec.message());
         do_close(beast::websocket::close_code::abnormal);
         do_read();
         return;
     }

     AUTOCRAFT_CONN_LOG("HANDSHAKED");

     do_read();
     state_ = state::authenticating;
 }

void connection::on_read(boost::system::error_code const& ec, std::size_t const& bytes_transferred) noexcept
{
    AUTOCRAFT_LOCK(handlers_mutex_);

    assert(state_ != state::closed);

    if (ec)
    {
        if (socket_.is_open())
        {
            AUTOCRAFT_CONN_LOG("READ ERROR: " << ec.message() << ", code: " << ec.value());
            do_close(beast::websocket::close_code::abnormal);
            do_read();
        }
        else
        {
            AUTOCRAFT_CONN_LOG("READ: SESSION CLOSED");
            state_ = state::closed;
        }

        return;
    }
    else if (state_ == state::closing)
    {
        AUTOCRAFT_CONN_LOG("READ: Cannot read: Socket is closing, " << bytes_transferred << " bytes");
        read_buffer_.consume(read_buffer_.size());
        return;
    }
    /*else if (state_ == loading_player)
    {
        CONN_LOG("READ: ERROR: NOT SUPPOSED TO RECEIVE DATA WHILE LOADING PLAYER ENTITY");
        do_close(beast::websocket::close_code::abnormal);
        do_read();
        return;
    }*/

    if (io_log && data_log)
        AUTOCRAFT_CONN_LOG("READ " << bytes_transferred << " BYTES: " << std::endl << get_readable(read_buffer_.data()));
    else if (io_log)
        AUTOCRAFT_CONN_LOG("READ " << bytes_transferred << " BYTES");

    std::string order_str = boost::beast::buffers_to_string(read_buffer_.data());
    read_buffer_.consume(read_buffer_.size());

    if (state_ == state::authenticating)
        interpret(std::move(order_str));
    else
        do_read();
}

 void connection::on_write(beast::error_code const& ec, std::size_t const& bytes_transferred) noexcept
 {
     AUTOCRAFT_LOCK(handlers_mutex_);

     auto msg = to_write_.front();
     to_write_.pop_front();

     if (ec)
     {
         if (ec.value() == 995)
             AUTOCRAFT_CONN_LOG("WRITE ERROR: operation aborted");
         else
             AUTOCRAFT_CONN_LOG("WRITE ERROR: " << ec.message());
         return;
     }

     /*if (state_ == to_be_closed)
     {
         CONN_LOG("ON WRITE: Session should be closed, closing it");
         do_close(beast::websocket::close_code::normal);
         return;
     }*/

     if (state_ != state::writing)
         return;

     state_ = state::reading;

     if (io_log && data_log)
         AUTOCRAFT_CONN_LOG("ON WRITE: " << bytes_transferred << " WRITTEN: " << std::endl << *msg);
     else if (io_log)
         AUTOCRAFT_CONN_LOG("ON WRITE: " << bytes_transferred << " WRITTEN");

     write_next();
 }

 void connection::on_close() noexcept
 {
     AUTOCRAFT_LOCK(handlers_mutex_);
     if (close_timer_.cancel() == 1)
         AUTOCRAFT_CONN_LOG("ON CLOSE: GRACEFUL CLOSE");
     else
         AUTOCRAFT_CONN_LOG("ON CLOSE: FORCED CLOSE");
 }

 void connection::do_read()
 {
     socket_.async_read(read_buffer_, asio::bind_executor(strand_, std::bind(&player_conn::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
 }

 void connection::do_close(beast::websocket::close_code const& code)
 {
     AUTOCRAFT_LOCK(handlers_mutex_);

     if (state_ == state::closed || state_ == state::closing)
     {
         AUTOCRAFT_CONN_LOG("WARNING: TRYING TO CLOSE BUT SOCKET IS ALREADY IN " << (state_ == state::closed ? "CLOSED" : "CLOSING") << " STATE");
         return;
     }

     if (state_ == state::writing)
     {
         AUTOCRAFT_CONN_LOG("CLOSE: Cannot close socket yet, there is a pending write");
         state_ = state::to_be_closed;
         close_code_ = code;
         return;
     }

     AUTOCRAFT_CONN_LOG("CLOSING");

     // If the endpoint does not acknowledge the close after N seconds, we force it by closing the underlying tcp socket
     close_timer_.expires_after(std::chrono::seconds(10));
     std::shared_ptr<connection> this_p = shared_from_this();
     close_timer_.async_wait([this, this_p](boost::system::error_code const& error) {
         AUTOCRAFT_LOCK(handlers_mutex_);
         if (state_ != state::closed && socket_.next_layer().is_open())
         {
             AUTOCRAFT_CONN_LOG("WEBSOCKET CLOSE DID NOT SUCCEED. CLOSING THE TCP SOCKET");
             socket_.next_layer().shutdown(asio::ip::tcp::socket::shutdown_both);
             socket_.next_layer().close();
         }
     });

     socket_.async_close(code, asio::bind_executor(strand_, std::bind(&player_conn::on_close, shared_from_this())));
     state_ = state::closing;
 }

 void connection::follow_up()
 {
     AUTOCRAFT_LOCK(handlers_mutex_);
     finish_auth();
     do_read();
     state_ = state::reading;
 }

 void connection::interpret(std::string &&order_str)
 {
     AUTOCRAFT_CONN_LOG("INTERPRETING " << order_str);

     try {
         nlohmann::json receivedJSON = nlohmann::json::parse(std::move(order_str));

         if (state_ == state::authenticating)
         {
             std::string playerName = receivedJSON["playerName"];

             if (playerName == "master")
             {
                 server_->register_master(shared_from_this());
             }
             else
                 server_->register_player(shared_from_this());

             return;

             AUTOCRAFT_CONN_LOG("Authentication: player name: " << playerName);

             //std::string player_name = j["player_name"];
             //if (server_->is_player_connected(player_name))
             //    throw std::runtime_error("AUTHENTICATION: PLAYER " + player_name + " ALREADY CONNECTED");

             //if (player_name.empty())
             //    throw std::runtime_error("AUTHENTICATION: INVALID PLAYER NAME");

             //player_name_ = player_name;

             //CONN_LOG("LOADING PLAYER " << player_name_);
             //server_->get_persistence()->async_load_player(player_name_, std::bind(&player_conn::on_player_load, shared_from_this(), std::placeholders::_1));
             //state_ = loading_player;

         }
         //else if (order == "action")
         //{
         //    std::string suborder = j["suborder"];
         //    AUTOCRAFT_LOCK(patches_mutex_);

         //    if (suborder == "change_speed")
         //        push_patch("speed", any(j["speed"].get<double>()));
         //    else if (suborder == "change_dir")
         //        push_patch("dir", any(vector({ j["dir"]["x"].get<double>(), j["dir"]["y"].get<double>() })));
         //    else if (suborder == "move_to")
         //        push_patch("target_pos", any(vector({ j["target_pos"]["x"].get<double>(), j["target_pos"]["y"].get<double>() })));
         //    else
         //        throw std::runtime_error("UNKNOWN ACTION: " + suborder);
         //}
         //else
         //    throw std::runtime_error("UNKNOWN ORDER: " + order);
     }
     catch (std::exception const& e) {
         AUTOCRAFT_CONN_LOG("INTERPRET ERROR: " << e.what());
         //do_close(beast::websocket::close_code::abnormal);
     }
 }

} // namespace autocraft
