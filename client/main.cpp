#include "core/client.h"

int main(int argc, char* argv[]) {
    try {
        if(argc != 4) {
            std::cerr << "Usage: chat_client <host> <port> <nickname>\n";
            return 1;
        }
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        std::array<char, MAX_NICKNAME> nickname{};
        strcpy(nickname.data(), argv[3]);

        Client client(nickname, io_context, iterator);
        std::thread t([&io_context]() {
            io_context.run();
        });
        std::array<char, MAX_IP_PACK_SIZE> msg{};
        while(true) {
            memset(msg.data(), '\0', msg.size());
            if(!std::cin.getline(msg.data(), MAX_IP_PACK_SIZE - MAX_NICKNAME)) {
                std::cin.clear();
            }
            client.write(msg);
        }

        client.close();
        t.join();
    }catch(std::exception &ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
    return 0;
}