#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "session.h"

class Server {
    tcp::acceptor acceptor_;
    std::mutex clients_mutex_;
    std::unordered_map<std::string, std::shared_ptr<ChatSession>> clients_;

    void accept();

public:
    Server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint);
    void start();
};

#endif // SERVER_H