#include "server.h"

#include <iostream>
#include "../logger.h"

Server::Server(boost::asio::io_context &io_context,
               const tcp::endpoint &endpoint)
    : db_("users.db"), acceptor_(io_context, endpoint) {
}

void Server::start() {
    std::cout << "Server started" << std::endl;
    Logger::info("Server started");

    db_.createTable("users", "id INTEGER PRIMARY KEY, username TEXT, password TEXT, email TEXT");

    accept();
}

void Server::accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if(!ec) {
            Logger::info("Accepted connection");
            std::cout << "Accepted connection" << std::endl;
            std::make_shared<ChatSession>(std::move(socket), clients_, clients_mutex_, db_)->start();
        }
        accept();
    });
}
