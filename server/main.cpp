#include <iostream>
#include <list>
#include <boost/thread.hpp>
#include "core/server.h"
#include <thread>

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: chat_server <port> [<port> ...]" << std::endl;
            return 1;
        }

        std::shared_ptr<boost::asio::io_context> io_context(new boost::asio::io_context);
        boost::shared_ptr<boost::asio::io_context::work> work(new boost::asio::io_context::work(*io_context));
        boost::shared_ptr<boost::asio::io_context::strand> strand(new boost::asio::io_context::strand(*io_context));

        std::cout << "[" << std::this_thread::get_id() << "] server starts" << std::endl;

        std::list<std::shared_ptr<Server>> servers;
        for(int i = 1; i < argc; ++i) {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            std::shared_ptr<Server> a_server(new Server(*io_context, *strand, endpoint));
            servers.push_back(a_server);
        }

        boost::thread_group workers;
        for(int i = 0; i < 1; ++i) {
            auto *t = new boost::thread{ [io_context] { Time::WorkerThread::run(io_context); } };
#ifndef __linux__
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i, &cpuset);
            pthread_setaffinity_np(t->native_handle(), sizeof(cpu_set_t), &cpuset);
#endif
            workers.add_thread(t);
        }
        workers.join_all();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}