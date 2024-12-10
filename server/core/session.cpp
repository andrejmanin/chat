#include "session.h"

#include <iostream>
#include "../logger.h"

ChatSession::ChatSession(tcp::socket socket, std::unordered_map<std::string,
                             std::shared_ptr<ChatSession>> &clients, std::mutex &clients_mutex, UsersDatabase& db)
        : db_(db), socket_(std::move(socket)), clients_mutex_(clients_mutex), clients_(clients) {}

void ChatSession::start() {
    std::cout << "Start session" << std::endl;
    Logger::info("Start session");
    reg();
}

void ChatSession::reg() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(write_msgs_),
        [this, self](boost::system::error_code ec, size_t length) {
            if (!ec) {
                std::string data(write_msgs_.data(), strlen(write_msgs_.data()));
                auto pos1 = data.find(' ');
                auto pos2 = data.find(' ', pos1 + 1);
                username_ = data.substr(0, pos1);
                std::string email = data.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string password = data.substr(pos2 + 1);

                if (username_.empty() || email.empty() || password.empty()) {
                    Logger::error("Invalid input: username, email, or password is empty");
                    return;
                }

                if (!checkUserExistence(email)) {
                    std::string query = "INSERT INTO users (username, password, email) VALUES (?, ?, ?)";
                    sqlite3_stmt* stmt;
                    if (sqlite3_prepare_v2(db_.getDB(), query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_text(stmt, 1, username_.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_TRANSIENT);
                        if (sqlite3_step(stmt) == SQLITE_DONE) {
                            socket_.async_write_some(boost::asio::buffer("Registration successful"),
                            [this](boost::system::error_code ec, size_t) {
                                if (ec) {
                                    Logger::error("Error sending registration success message: " + ec.message());
                                }
                            });
                            clients_.emplace(username_, shared_from_this());
                            readMsg();
                        } else {
                        socket_.async_write_some(boost::asio::buffer("Registration failed"),
                        [this](boost::system::error_code ec, size_t) {
                            if (ec) {
                                Logger::error("Error sending registration success message: " + ec.message());
                            }
                        });
                    }
                    sqlite3_finalize(stmt);
                } else {
                    socket_.async_write_some(boost::asio::buffer("This email is already registered"),
                        [this](boost::system::error_code ec, size_t) {
                            if (ec) {
                                Logger::error("Error sending registration success message: " + ec.message());
                            }
                        });
                }
                } else {
                    socket_.async_write_some(boost::asio::buffer("This email is already registered"),
                        [this](boost::system::error_code ec, size_t) {
                            if (ec) {
                                Logger::error("Error sending email already registered message: " + ec.message());
                            }
                        });
                }
        }
    });

}

bool ChatSession::checkUserExistence(const std::string& email) const {
    bool isExist = false;
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        if (argc > 0) {
            *(static_cast<bool*>(data)) = true;
        }
        return 0;
    };
    db_.executeWithCallback("SELECT username FROM users WHERE email = '" + email + "'", callback, &isExist);
    return isExist;
}

void ChatSession::readMsg() {
    auto self(shared_from_this());
    write_msgs_.fill('\000');
    socket_.async_read_some(boost::asio::buffer(write_msgs_),
        [this, self](boost::system::error_code ec, size_t lenght) {
            if(!ec) {
                Logger::info("Get message from " + username_ + ' ' + std::string(write_msgs_.data(), lenght));
                if(std::string(write_msgs_.data(), 5) == "/list") {
                    std::string clients;
                    db_.executeWithCallback("SELECT username FROM users", [](void* data, int argc, char** argv, char** azColName) -> int {
                        auto* clients = static_cast<std::string*>(data);
                        if (argc > 0) {
                            if (!clients->empty()) {
                                *clients += ", ";
                            }
                            *clients += (argv[0] ? std::string(argv[0]) : "Unknown");
                        }
                        return 0;
                    }, &clients);
                    boost::asio::async_write(socket_, boost::asio::buffer(clients),
                        [this](boost::system::error_code ec, size_t lenght) {
                            if(ec) {
                                Logger::error("Error sending message: " + ec.message());
                                std::cerr << "Error sending message: " << ec.message() << std::endl;
                            }
                        });
                } else {
                    std::string msg = std::string(write_msgs_.data(), lenght);
                    auto pos = msg.find(':');
                    if(pos != std::string::npos) {
                        std::string recipient = msg.substr(0, pos);
                        std::string text = msg.substr(pos + 1);
                        sentMsg(recipient, text);
                    }
                }
                readMsg();
            }
        });
}

void ChatSession::sentMsg(const std::string& recipient, const std::string &text) const {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    auto it = clients_.find(recipient);
    if(it != clients_.end()) {
        Logger::info("Sent message to " + recipient);
        std::string msg = "@" + username_ + ":" + text;
        boost::asio::async_write(it->second->socket_, boost::asio::buffer(msg),
            [this, msg](boost::system::error_code ec, size_t lenght) {
                if(ec) {
                    Logger::error("Error sending message: " + ec.message());
                    std::cerr << "Error sending message: " << ec.message() << std::endl;
                }
            });
    }
}

std::string* ChatSession::getClients() const {
    auto* clients = new std::string();
    for(auto& client : clients_) {
        clients->append(client.first + "\n");
    }
    return clients;
}