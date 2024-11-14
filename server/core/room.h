#ifndef ROOM_H
#define ROOM_H

#include <boost/asio.hpp>
#include <array>
#include <deque>
#include <unordered_set>
#include "../../protocol.h"

using boost::asio::ip::tcp;

namespace Time{
    std::string getTimestamp();

    class WorkerThread {
    public:
        static void run(const std::shared_ptr<boost::asio::io_context> &io_context);
    private:
        static std::mutex mutex_;
    };
}


class IParticipant {
public:
    virtual ~IParticipant() = default;
    virtual void onMassage(std::array<char, MAX_IP_PACK_SIZE> &msg) = 0;
};

class ChatRoom {
public:
    void enter(std::shared_ptr<IParticipant> participant, const std::string &nickname);
    void leave(const std::shared_ptr<IParticipant>& participant);
    void broadcast(std::array<char, MAX_IP_PACK_SIZE> &msg, const std::shared_ptr<IParticipant> &participant);
    std::string getNickname(const std::shared_ptr<IParticipant>& participant);
private:
    const int MAX_RECENT_MESSAGES = 1000;
    std::unordered_map<std::shared_ptr<IParticipant>, std::string> room_;
    std::unordered_set<std::shared_ptr<IParticipant>> participants_;
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> recent_msgs_;
};

class PersonInRoom : public IParticipant, public std::enable_shared_from_this<PersonInRoom> {
public:
    explicit PersonInRoom(boost::asio::io_context &io_context, boost::asio::io_context::strand &strand, ChatRoom &chat_room)
    : socket_(io_context), strand_(strand), chat_room_(chat_room) {}

    tcp::socket &socket() { return socket_; }

    void start();
    void onMassage(std::array<char, MAX_IP_PACK_SIZE> &msg) override;

private:
    void nicknameHandler(const boost::system::error_code& error);
    void readHandler(const boost::system::error_code& error);
    void writeHandler(const boost::system::error_code& error);

    tcp::socket socket_;
    boost::asio::io_context::strand strand_;
    ChatRoom &chat_room_;
    std::array<char, MAX_NICKNAME> nickname_{};
    std::array<char, MAX_IP_PACK_SIZE> read_msg_{};
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> write_msg_{};
};


#endif //ROOM_H
