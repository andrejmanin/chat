#include "client.h"

Client::Client(std::array<char, MAX_NICKNAME> nickname, boost::asio::io_context& io_context, const tcp::resolver::iterator& endpoint_iterator)
        : io_context_(io_context), socket_(io_context) {
    strcpy(nickname_.data(), nickname.data());
    memset(read_msgs_.data(), '\0', MAX_IP_PACK_SIZE);
    boost::asio::async_connect(socket_, endpoint_iterator, [this](const boost::system::error_code& error, const tcp::resolver::iterator&) {
        this->onConnect(error);
    });
}

void Client::closeImpl() {
    socket_.close();
}

void Client::onConnect(const boost::system::error_code& error) {
    if (!error) {
        boost::asio::async_read(socket_, boost::asio::buffer(read_msgs_), [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            this->readHandler(error, bytes_transferred);
        });
    }
}

void Client::readHandler(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::cout << std::string(read_msgs_.data(), bytes_transferred) << std::endl;
        boost::asio::async_read(socket_, boost::asio::buffer(read_msgs_), [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            this->readHandler(error, bytes_transferred);
        });
    } else {
        closeImpl();
    }
}

void Client::writeImpl(const std::array<char, MAX_IP_PACK_SIZE> &msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if(!write_in_progress) {
        boost::asio::async_write(socket_, boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().size()), [this](const boost::system::error_code& error, std::size_t) {
            this->writeHandler(error);
        });
    }
}

void Client::writeHandler(const boost::system::error_code& error) {
    if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
            boost::asio::async_write(socket_, boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().size()), [this](const boost::system::error_code& error, std::size_t) {
                this->writeHandler(error);
            });
        }
    } else {
        closeImpl();
    }
}

void Client::write(const std::array<char, MAX_IP_PACK_SIZE>& msg) {
    io_context_.post([this, msg]() {
        this->writeImpl(msg);
    });
}

void Client::close() {
    io_context_.post([this]() {
       this->closeImpl();
    });
}