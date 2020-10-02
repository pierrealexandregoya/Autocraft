#include <iostream>
#include <boost/asio.hpp>

void connect_to_unity()
{
    boost::asio::io_context io_context;

    boost::asio::ip::tcp::resolver resolver(io_context);

    boost::asio::ip::tcp::resolver::results_type endpoints =
	resolver.resolve("localhost", "11000");

    boost::asio::ip::tcp::socket socket(io_context);

    boost::system::error_code error;
    boost::asio::connect(socket, endpoints, error);
    if (error)
    {
	std::cout << error.message() << std::endl;
	return 1;
    }
}
