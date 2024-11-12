#include "server.h"

#include <boost/bind/bind.hpp>

void Server::run() {
    std::shared_ptr<PersonInRoom> new_participant(new PersonInRoom(io_context_, strand_, room_));
    acceptor_.async_accept(new_participant->socket(),
        strand_.wrap(strand_.wrap([this, new_participant](const boost::system::error_code &error) {
            onAccept(new_participant, error);
        })));
}

void Server::onAccept(std::shared_ptr<PersonInRoom> new_participant, const boost::system::error_code &error) {
    if (!error) {
        new_participant->start();
    }
    run();
}

