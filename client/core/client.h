#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include <boost/asio.hpp>
#include "../../parameters.h"

using boost::asio::ip::tcp;

class Client {
    const std::string IP_ADDR = "127.0.0.1";

    std::mutex mutex_;
    tcp::socket socket_;
    boost::asio::io_context& io_context_;
    std::array<char, MAX_IP_PACK_SIZE> read_msgs_{};
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> write_msg_;

    void registerUser();
    void loginUser();
    void readMsg();
    void writeMsg(std::array<char, MAX_IP_PACK_SIZE> msg);
    void writeHandler();
public:
    Client(boost::asio::io_context& io_context, tcp::resolver& resolver);
    void write(const std::array<char, MAX_IP_PACK_SIZE>& msg);
    void close();
};

#endif // CLIENT_H