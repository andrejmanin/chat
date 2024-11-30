#include <iostream>

#include "core/client.h"

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        resolver.resolve(tcp::v4(), "127.0.0.1", std::to_string(PORT));
        Client client(io_context, resolver);

        std::thread t([&io_context](){ io_context.run(); });

        std::thread input_thread([&client]() {
            std::string input;
            while (std::getline(std::cin, input)) {
                if (!input.empty()) {
                    std::array<char, MAX_IP_PACK_SIZE> msg{};
                    std::copy(input.begin(), input.end(), msg.begin());
                    client.write(msg);
                }
            }
        });

        t.join();
        input_thread.join();


    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
}