#ifndef SERVER_H
#define SERVER_H

#include "room.h"

using boost::asio::ip::tcp;


class Server {
public:
    Server(boost::asio::io_context &io_context, boost::asio::io_context::strand &strand, const tcp::endpoint endpoint)
        : io_context_(io_context), strand_(strand), acceptor_(io_context, endpoint) {
        run();
    }


private:
    void run();
    void onAccept(std::shared_ptr<PersonInRoom> new_participant, const boost::system::error_code &error);

    boost::asio::io_context& io_context_;
    boost::asio::io_context::strand& strand_;
    tcp::acceptor acceptor_;
    ChatRoom room_;
};



#endif //SERVER_H
