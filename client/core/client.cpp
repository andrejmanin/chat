#include "client.h"

#include <iostream>
#include "interface.h"

Client::Client(boost::asio::io_context &io_context, tcp::resolver& resolver)
    : socket_(io_context), io_context_(io_context) {

    boost::asio::async_connect(socket_, resolver.resolve(IP_ADDR, std::to_string(PORT)),
        [this](boost::system::error_code ec, const tcp::endpoint&) {
            if(!ec) {
                sendName();
            } else {
                Interface::Colors::error();
                std::cout << "Connection failed: " << ec.message() << std::endl;
            }
        });
}

void Client::sendName() {
    std::string username;
    Interface::Info::start();
    Interface::Colors::send();
    std::getline(std::cin, username);
    Interface::Colors::resetColor();

    socket_.async_send(boost::asio::buffer(username),
        [this, username](boost::system::error_code ec, size_t) {
            if (!ec) {
                Interface::Info::sentUsername();
                Interface::Info::msgInfo();
                readMsg();
            } else {
                Interface::Colors::error();
                std::cerr << "Error sending username: " << ec.message() << std::endl;
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