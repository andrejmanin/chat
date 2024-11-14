#include "room.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

std::mutex Time::WorkerThread::mutex_;

std::string getTimestamp() {
    const time_t t = time(nullptr);
    const struct tm *tmNow = localtime(&t);
    std::stringstream ss;
    ss << '[' << (tmNow->tm_year + 1900) << '-' << std::setfill('0')
    << std::setw(2) << (tmNow->tm_mon + 1) << '-' << std::setfill('0')
    << std::setw(2) << tmNow->tm_mday << ' ' << std::setfill('0')
    << std::setw(2) << tmNow->tm_hour << ':' << std::setfill('0')
    << std::setw(2) << tmNow->tm_min << ']';

    return ss.str();
}

void Time::WorkerThread::run(const std::shared_ptr<boost::asio::io_context> &io_context) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[" << std::this_thread::get_id() << "] Thread starts" << std::endl;
    }

    io_context->run();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[" << std::this_thread::get_id() << "] Thread ends" << std::endl;
    }
}




void ChatRoom::enter(std::shared_ptr<IParticipant> participant, const std::string &nickname) {
    participants_.insert(participant);
    room_[participant] = nickname;
    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                  [&participant](std::array<char, MAX_IP_PACK_SIZE>& msg) {
                      participant->onMassage(msg);
                  });
}

void ChatRoom::leave(const std::shared_ptr<IParticipant>& participant) {
    participants_.erase(participant);
    room_.erase(participant);
}

void ChatRoom::broadcast(std::array<char, MAX_IP_PACK_SIZE> &msg, const std::shared_ptr<IParticipant> &participant) {
    std::string timestamp = getTimestamp();
    std::string nickname = getNickname(participant);
    std::array<char, MAX_IP_PACK_SIZE> formatted_msg{};

    strcpy(formatted_msg.data(), timestamp.c_str());
    strcat(formatted_msg.data(), nickname.c_str());
    strcat(formatted_msg.data(), msg.data());

    recent_msgs_.push_back(formatted_msg);
    while (recent_msgs_.size() > MAX_RECENT_MESSAGES) {
        recent_msgs_.pop_front();
    }

    std::for_each(participants_.begin(), participants_.end(),
                  [&formatted_msg](const std::shared_ptr<IParticipant>& p) {
                      p->onMassage(formatted_msg);
                  });
}

std::string ChatRoom::getNickname(const std::shared_ptr<IParticipant>& participant) {
    return room_[participant];
}

void PersonInRoom::nicknameHandler(const boost::system::error_code &error) {
    if (!error) {
        if (strlen(nickname_.data()) <= MAX_NICKNAME - 2) {
            strcat(nickname_.data(), ": ");
        } else {
            nickname_[MAX_NICKNAME - 2] = ':';
            nickname_[MAX_NICKNAME - 1] = ' ';
        }
        chat_room_.enter(shared_from_this(), std::string(nickname_.data()));
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_, read_msg_.size()),
            strand_.wrap([shared_this = shared_from_this()](const boost::system::error_code &error, const size_t &bytes_transferred) {
                shared_this->readHandler(error);
            }));
    } else {
        chat_room_.leave(shared_from_this());
    }
}

void PersonInRoom::readHandler(const boost::system::error_code &error) {
    if(!error) {
        chat_room_.broadcast(read_msg_, shared_from_this());
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_, read_msg_.size()),
            strand_.wrap([self = shared_from_this()](const boost::system::error_code &error, const size_t &bytes_transferred) {
                self->readHandler(error);
            }));
    } else {
        chat_room_.leave(shared_from_this());
    }
}

void PersonInRoom::writeHandler(const boost::system::error_code &error) {
    if(!error) {
        write_msg_.pop_front();
        if(!write_msg_.empty()) {
            boost::asio::async_write(socket_,
                boost::asio::buffer(write_msg_.front(), write_msg_.front().size()),
                strand_.wrap([self = shared_from_this()](const boost::system::error_code &error, const size_t &bytes_transferred) {
                    self->writeHandler(error);
                }));
        }
    } else {
        chat_room_.leave(shared_from_this());
    }
}

void PersonInRoom::onMassage(std::array<char, MAX_IP_PACK_SIZE> &msg){
    const bool msgsQueue = !write_msg_.empty();
    write_msg_.push_back(msg);
    if(!msgsQueue) {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msg_.front(), write_msg_.front().size()),
            strand_.wrap([self = shared_from_this()](const boost::system::error_code &error, const size_t &bytes_transferred) {
                self->writeHandler(error);
            }));
    }
}

void PersonInRoom::start() {
    boost::asio::async_read(socket_,
        boost::asio::buffer(nickname_, nickname_.size()),
        strand_.wrap([self = shared_from_this()](const boost::system::error_code &error, const size_t &bytes_transferred) {
            self->nicknameHandler(error);
        }));
}



