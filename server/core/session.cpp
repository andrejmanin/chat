#include "session.h"

#include <iostream>

ChatSession::ChatSession(tcp::socket socket, std::unordered_map<std::string,
                             std::shared_ptr<ChatSession>> &clients, std::mutex &clients_mutex)
        : socket_(std::move(socket)), clients_mutex_(clients_mutex), clients_(clients) {}

void ChatSession::start() {
    std::cout << "Start session" << std::endl;
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
                std::cout << "Got username: " << username_ << std::endl;
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
                std::cout << "Get message from: " << username_ << " message: " << std::string(write_msgs_.data(), lenght) << std::endl;
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
        std::cout << "Sent message to " << recipient << std::endl;
        std::string msg = "@" + username_ + ": " + text;
        boost::asio::async_write(it->second->socket_, boost::asio::buffer(msg),
            [this, msg](boost::system::error_code ec, size_t lenght) {
                if(ec) {
                    std::cerr << "Error sending message: " << ec.message() << std::endl;
                }
                std::cout << "Sent next message: " << msg << std::endl;
            });
    }
}