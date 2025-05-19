#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

void client() {
    try {
        boost::asio::io_context io_context;

        // Listen on port 12345
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Server is running on port 12345..." << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::cout << "Client connected." << std::endl;

            for (;;) {
                char data[1024] = { 0 };
                boost::system::error_code error;
                std::size_t length = socket.read_some(boost::asio::buffer(data), error);

                if (error == boost::asio::error::eof) break; // connection closed cleanly
                else if (error) throw boost::system::system_error(error); // other error

                std::cout << "Received: " << data << std::endl;

                boost::asio::write(socket, boost::asio::buffer(data, length));
            }

            std::cout << "Client disconnected." << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}