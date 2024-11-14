#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <array>
#include <deque>
#include <thread>
#include "../../protocol.h"
#include <iostream>

using boost::asio::ip::tcp;

class Client {
private:
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    std::array<char, MAX_NICKNAME> nickname_{};
    std::array<char, MAX_IP_PACK_SIZE> read_msgs_{};
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> write_msgs_;

    void closeImpl();

    void onConnect(const boost::system::error_code& error);

    void readHandler(const boost::system::error_code& error, std::size_t bytes_transferred);

    void writeImpl(const std::array<char, MAX_IP_PACK_SIZE> &msg);

    void writeHandler(const boost::system::error_code& error);
public:
    Client(std::array<char, MAX_NICKNAME> nickname, boost::asio::io_context& io_context, const tcp::resolver::iterator& endpoint_iterator);

    void write(const std::array<char, MAX_IP_PACK_SIZE>& msg);

    void close();

};



#endif //CLIENT_H