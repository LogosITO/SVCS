#include <iostream>
#include <array>
#include <boost/asio.hpp>


int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }
        boost::asio::io_context io;
        boost::asio::ip::tcp::resolver r(io);
        boost::asio::ip::tcp::resolver::results_type endpoints = r.resolve(argv[1], "daytime");
        boost::asio::ip::tcp::socket socket(io);
        boost::asio::connect(socket, endpoints);
        for(;;){
            std::array<char, 128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);

            std::cout.write(buf.data(), len);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}