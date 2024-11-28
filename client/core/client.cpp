#include "client.h"

#include <iostream>

Client::Client(boost::asio::io_context &io_context, tcp::resolver& resolver)
    : socket_(io_context), io_context_(io_context) {

    boost::asio::async_connect(socket_, resolver.resolve(IP_ADDR, std::to_string(PORT)),
        [this](boost::system::error_code ec, const tcp::endpoint&) {
            if(!ec) {
                std::cout << "Connected to server" << std::endl;
                sendName();
            } else {
                std::cout << "Connection failed: " << ec.message() << std::endl;
            }
        });
}

void Client::sendName() {
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    socket_.async_send(boost::asio::buffer(username),
        [this, username](boost::system::error_code ec, size_t) {
            if (!ec) {
                std::cout << "Username sent to server: " << username << std::endl;
                readMsg();
            } else {
                std::cerr << "Error sending username: " << ec.message() << std::endl;
            }
        });
}


void Client::readMsg() {
    std::cout << read_msgs_.data() << std::endl;
    boost::asio::async_read(socket_, boost::asio::buffer(read_msgs_),
        [this](boost::system::error_code ec, size_t length) {
            if (!ec) {
                readMsg();
            } else {
                std::cerr << "Error reading message: " << ec.message() << std::endl;
            }
        });
}

void Client::write(const std::array<char, MAX_IP_PACK_SIZE> &msg) {
    io_context_.post([this, msg]() {
        writeMsg(msg);
    });
}


void Client::writeMsg(std::array<char, MAX_IP_PACK_SIZE> msg) {
    boost::asio::async_write(socket_, boost::asio::buffer(write_msg_.front()),
        [this, msg](boost::system::error_code ec, size_t) {
            if (!ec) {
                bool write_in_progress = !write_msg_.empty();
                write_msg_.push_back(msg);
                if (!write_in_progress) {
                    boost::asio::async_write(socket_, boost::asio::buffer(write_msg_.front(), write_msg_.front().size()),
                        [this](boost::system::error_code ec, size_t) {
                            if (!ec) {
                                write_msg_.pop_front();
                                if (!write_msg_.empty()) {
                                    writeHandler();
                                }
                            } else {
                                std::cerr << "Error writing message: " << ec.message() << std::endl;
                            }
                        });
                }
            } else {
                std::cerr << "Error writing message: " << ec.message() << std::endl;
            }
        });
}

void Client::writeHandler() {
    boost::asio::async_write(socket_, boost::asio::buffer(write_msg_.front(), write_msg_.front().size()),
        [this](boost::system::error_code ec, size_t) {
            if (!ec) {
                write_msg_.pop_front();
                if (!write_msg_.empty()) {
                    writeHandler();
                }
            } else {
                std::cerr << "Error writing message: " << ec.message() << std::endl;
            }
        });
}


