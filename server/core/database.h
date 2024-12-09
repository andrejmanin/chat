#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
protected:
    sqlite3* db_;
    std::string db_name_;
public:
    Database(const std::string& db_name);
    virtual ~Database() = 0;

    virtual bool execute(const std::string& query) = 0;
    virtual bool executeWithCallback(const std::string& query, int (*callback)(void*, int, char**, char**), void* data) = 0;
    virtual bool createTable(const std::string& table_name, const std::string& columns) = 0;
};

class UsersDatabase : protected Database {
public:
    explicit UsersDatabase(const std::string& db_name);
    ~UsersDatabase() override;
    bool execute(const std::string &query) override;
    bool executeWithCallback(const std::string &query, int (*callback)(void *, int, char **, char **), void *data) override;
    bool createTable(const std::string& table_name, const std::string& columns) override;
    sqlite3* getDB() const { return db_; }

};

#endif //DATABASE_H
