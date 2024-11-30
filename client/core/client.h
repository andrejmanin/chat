#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include <boost/asio.hpp>
#include "../../parameters.h"

using boost::asio::ip::tcp;

class Client {
    const std::string IP_ADDR = "127.0.0.1";

    tcp::socket socket_;
    boost::asio::io_context& io_context_;
    std::array<char, MAX_IP_PACK_SIZE> read_msgs_{};
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> write_msg_;

    void sendName();
    void readMsg();
    void writeMsg(std::array<char, MAX_IP_PACK_SIZE> msg);
    void writeHandler();
public:
    Client(boost::asio::io_context& io_context, tcp::resolver& resolver);
    void write(const std::array<char, MAX_IP_PACK_SIZE>& msg);
};

#endif // CLIENT_H