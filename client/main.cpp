#include <iostream>

#include "core/client.h"
#include "core/interface.h"

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        resolver.resolve(tcp::v4(), "127.0.0.1", std::to_string(PORT));
        Client client(io_context, resolver);

        std::thread t([&io_context](){ io_context.run(); });

        std::thread input_thread([&client]() {
            std::string input;
            while (Interface::Colors::send(), std::getline(std::cin, input)) {
                if (!input.empty()) {
                    if(input == "/exit") {
                        Interface::Info::close();
                        client.close();
                        break;
                    }
                    if(input == "/info") {
                        Interface::Info::info();
                    } else {
                        if(input == "/list") {
                            Interface::Info::getClients();
                        }
                        std::array<char, MAX_IP_PACK_SIZE> msg{};
                        std::copy(input.begin(), input.end(), msg.begin());
                        client.write(msg);
                    }
                }
            }
        });

        t.join();
        input_thread.join();


    } catch (std::exception& exception) {
        Interface::Colors::error();
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
}
