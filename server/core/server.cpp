#include "server.h"

#include <iostream>
#include "../logger.h"

Server::Server(boost::asio::io_context &io_context,
               const tcp::endpoint &endpoint)
        : acceptor_(io_context, endpoint) {}

void Server::start() {
    std::cout << "Server started" << std::endl;
    Logger::info("Server started");
    accept();
}

void Server::accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if(!ec) {
            Logger::info("Accepted connection");
            std::cout << "Accepted connection" << std::endl;
            std::make_shared<ChatSession>(std::move(socket), clients_, clients_mutex_)->start();
        }
        accept();
    });
}
