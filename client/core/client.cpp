#include "client.h"

#include <iostream>
#include "interface.h"

Client::Client(boost::asio::io_context &io_context, tcp::resolver& resolver)
    : socket_(io_context), io_context_(io_context) {

    boost::asio::async_connect(socket_, resolver.resolve(IP_ADDR, std::to_string(PORT)),
        [this](boost::system::error_code ec, const tcp::endpoint&) {
            if(!ec) {
                Interface::Info::start();
                registerUser();
            } else {
                Interface::Colors::error();
                std::cerr << "Connection failed: " << ec.message() << std::endl;
            }
        });
}

void Client::registerUser() {
    std::string username, email, password;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    std::cout << "Enter your email: ";
    std::getline(std::cin, email);
    std::cout << "Enter your password: ";
    std::getline(std::cin, password);

    std::string data = username + " " + email + " " + password;
    socket_.async_send(boost::asio::buffer(data),
        [this](boost::system::error_code ec, size_t) {
            if (!ec) {
                readMsg();
            } else {
                Interface::Colors::error();
                std::cerr << "Error sending registration data: " << ec.message() << std::endl;
            }
        });
}

void Client::readMsg() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Interface::Colors::receive();
        std::cout << read_msgs_.data() << std::endl;
    }
    Interface::Colors::resetColor();
    socket_.async_read_some(boost::asio::buffer(read_msgs_),
            [this](boost::system::error_code ec, size_t length) {
            if (!ec) {
                readMsg();
            } else {
                Interface::Colors::error();
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
                                Interface::Colors::error();
                                std::cerr << "Error writing message: " << ec.message() << std::endl;
                            }
                        });
                }
            } else {
                Interface::Colors::error();
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
                Interface::Colors::error();
                std::cerr << "Error writing message: " << ec.message() << std::endl;
            }
        });
}

void Client::close() {
    io_context_.stop();
    socket_.close();
}