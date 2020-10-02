#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/websocket/stream.hpp>

#include "any.hpp"
#include "noncopyable.hpp"
#include "persistence.hpp"

#define AUTOCRAFT_CONN_LOG(to_log) AUTOCRAFT_LOG(addr_str, to_log)

namespace autocraft {

class player;
class server;

class connection : public std::enable_shared_from_this<connection>
{
protected:
    enum class state
    {
        none,
        ready,
         handshaking,
        authenticating,
        //loading_player,
        reading,
        writing,
        to_be_closed,
        closing,
        closed
    };

public:
    static std::vector<std::string> const                          state_str;

    std::string const                                              addr_str;

protected:
     boost::beast::websocket::stream<boost::asio::ip::tcp::socket> socket_;
    boost::asio::io_context::strand                                strand_;
    state                                                          state_;
     boost::beast::multi_buffer                                    read_buffer_;
    std::list<std::shared_ptr<std::string const>>                  to_write_;
#ifndef AUTOCRAFT_MONOTHREAD
    mutable std::recursive_mutex                                   handlers_mutex_;
    mutable std::recursive_mutex                                   patches_mutex_;
#endif /* !AUTOCRAFT_MONOTHREAD */
     boost::beast::websocket::close_code                           close_code_;
    std::shared_ptr<server>                                        server_;
    boost::asio::steady_timer                                      close_timer_;

//private:
//    AUTOCRAFT_NON_COPYABLE(connection);

public:
    connection(boost::asio::ip::tcp::socket &&socket, boost::asio::io_context &ioc, std::shared_ptr<server> const& server);
    connection(connection&& other);
    connection(connection const & other) = delete;
    ~connection();

    void  start();
    void  write(std::shared_ptr<std::string const> msg);
    void  close();
    bool  is_closed() const;
    state current_state() const;

    void follow_up();

private:
    void write_next();
    void on_accept(boost::system::error_code const& ec) noexcept;
    void on_read(boost::system::error_code const& error, std::size_t const& bytes_transferred) noexcept;
    void on_write(boost::system::error_code const& ec, std::size_t const& bytes_transferred) noexcept;
    void on_close() noexcept;
    void do_read();
    void do_close(boost::beast::websocket::close_code const& code);

    virtual void interpret(std::string &&order_str);

    virtual void finish_auth() {};
};

} // namespace autocraft
