#include "session.h"

#include <iostream>
#include "../logger.h"

ChatSession::ChatSession(tcp::socket socket, std::unordered_map<std::string,
                             std::shared_ptr<ChatSession>> &clients, std::mutex &clients_mutex)
        : socket_(std::move(socket)), clients_mutex_(clients_mutex), clients_(clients) {}

void ChatSession::start() {
    std::cout << "Start session" << std::endl;
    Logger::info("Start session");
    readUsrName();
}

void ChatSession::readUsrName() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(write_msgs_),
        [this, self](boost::system::error_code ec, size_t lenght) {
            if(!ec) {
                username_ = std::string(write_msgs_.data(), lenght);
                {
                    std::lock_guard<std::mutex> lock(clients_mutex_);
                   clients_.emplace(username_, shared_from_this());
                }
                Logger::info("Got username: " + username_);
                readMsg();
            }
        });
}

void ChatSession::readMsg() {
    auto self(shared_from_this());
    write_msgs_.fill('\000');
    socket_.async_read_some(boost::asio::buffer(write_msgs_),
        [this, self](boost::system::error_code ec, size_t lenght) {
            if(!ec) {
                Logger::info("Get message from " + username_ + std::string(write_msgs_.data(), lenght));
                // std::lock_guard<std::mutex> lock(clients_mutex_);
                std::string msg = std::string(write_msgs_.data(), lenght);
                auto pos = msg.find(':');
                if(pos != std::string::npos) {
                    std::string recipient = msg.substr(0, pos);
                    std::string text = msg.substr(pos + 1);
                    sentMsg(recipient, text);
                }
                readMsg();
            }
        });
}

void ChatSession::sentMsg(const std::string& recipient, const std::string &text) const {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    auto it = clients_.find(recipient);
    if(it != clients_.end()) {
        Logger::info("Sent message to " + recipient);
        std::string msg = "@" + username_ + ": " + text;
        boost::asio::async_write(it->second->socket_, boost::asio::buffer(msg),
            [this, msg](boost::system::error_code ec, size_t lenght) {
                if(ec) {
                    Logger::error("Error sending message: " + ec.message());
                    std::cerr << "Error sending message: " << ec.message() << std::endl;
                }
            });
    }
}