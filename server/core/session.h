#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <mutex>
#include <deque>

#include "../../parameters.h"

using boost::asio::ip::tcp;

class ChatSession : public std::enable_shared_from_this<ChatSession> {
    tcp::socket socket_;
    std::string username_;
    std::mutex& clients_mutex_;
    std::array<char, MAX_IP_PACK_SIZE> write_msgs_{};
    std::unordered_map<std::string, std::shared_ptr<ChatSession>>& clients_;

    std::string* getClients() const;

    void readUsrName();
    void readMsg();
    void sentMsg(const std::string& recipient, const std::string &text) const;

public:
    ChatSession(tcp::socket socket, std::unordered_map<std::string, std::shared_ptr<ChatSession>>& clients, std::mutex& clients_mutex);
    void start();
};

#endif // SESSION_H