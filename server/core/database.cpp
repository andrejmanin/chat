/*
#include "database.h"
#include "../logger.h"

#include <iostream>

Database::Database(const std::string &db_name) : db_(nullptr), db_name_(db_name) {
    if(sqlite3_open(db_name.c_str(), &db_) != SQLITE_OK) {
        Logger::error("Can't open database: " + std::string(sqlite3_errmsg(db_)));
        sqlite3_close(db_);
        throw std::runtime_error("Failed open database: " + std::string(sqlite3_errmsg(db_)));
    }
    Logger::info("Database opened successfully");
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
        Logger::info("Database closed");
    }
}

UsersDatabase::UsersDatabase(const std::string &db_name) : Database(db_name) {}

UsersDatabase::~UsersDatabase() = default;

bool UsersDatabase::execute(const std::string &query) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
        Logger::error("SQL error: " + std::string(err_msg));
        sqlite3_free(err_msg);
        throw std::runtime_error("SQL error: " + std::string(err_msg));
    }
    Logger::info("SQL query executed successfully");
    return true;
}

void UsersDatabase::executeWithCallback(const std::string &query, int (*callback)(void *, int, char **, char **), void *data) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db_, query.c_str(), callback, nullptr, &err_msg) != SQLITE_OK) {
        Logger::error("SQL error executing query with callback: " + std::string(err_msg));
        sqlite3_free(err_msg);
        throw std::runtime_error("SQL error executing query with callback: " + std::string(err_msg));
    }
}

bool UsersDatabase::createTable(const std::string& table_name, const std::string& columns) {
    std::string query = "CREATE TABLE IF NOT EXITS " + table_name + " (" + columns + ");";
    return execute(query);
}
*/


#include "database.h"
#include "../logger.h"

#include <iostream>

Database::Database(const std::string &db_name) : db_(nullptr), db_name_(db_name) {
    if(sqlite3_open(db_name.c_str(), &db_) != SQLITE_OK) {
        Logger::error("Can't open database: " + std::string(sqlite3_errmsg(db_)));
        sqlite3_close(db_);
        throw std::runtime_error("Failed open database: " + std::string(sqlite3_errmsg(db_)));
    }
    Logger::info("Database opened successfully");
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
        Logger::info("Database closed");
    }
}

UsersDatabase::UsersDatabase(const std::string &db_name) : Database(db_name) {}

UsersDatabase::~UsersDatabase() = default;

bool UsersDatabase::execute(const std::string &query) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
        Logger::error("SQL error: " + std::string(err_msg));
        sqlite3_free(err_msg);
        return false;
    }
    Logger::info("SQL query executed successfully");
    return true;
}

bool UsersDatabase::executeWithCallback(const std::string &query, int (*callback)(void *, int, char **, char **), void *data) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db_, query.c_str(), callback, data, &err_msg) != SQLITE_OK) {
        Logger::error("SQL error executing query with callback: " + std::string(err_msg));
        sqlite3_free(err_msg);
        std::cerr << "SQL error executing query with callback: " + std::string(err_msg) << std::endl;
    }
    return true;
}

bool UsersDatabase::createTable(const std::string& table_name, const std::string& columns) {
    std::string query = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + columns + ");";
    return execute(query);
}

