#include <iostream>

#include "core/server.h"

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), PORT);
        Server server(io_context, endpoint);
        server.start();
        io_context.run();
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
    return 0;
}