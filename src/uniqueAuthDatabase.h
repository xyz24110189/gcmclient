#ifndef UNIQUEAUTHDATABASE_H
#define UNIQUEAUTHDATABASE_H

#include "commontype.h"
#include <sqlite3.h>
#include <string>

class UniqueAuthDatabase
{
public:
    UniqueAuthDatabase();
    ~UniqueAuthDatabase();

    UniqueAuthDatabase(const UniqueAuthDatabase &other) = delete;
    UniqueAuthDatabase &operator=(const UniqueAuthDatabase &other) = delete;

    bool InitDb();
    bool InsertTGT(const AppData &data, const std::string &strTGT);
    bool QueryLocalTGT(const AppData &data, std::string &strTGT);
    bool GetLocalTGT(std::string &strTGT);
    bool DeleteTGT();

protected:
    bool CreateTable();
    bool TableExistJudge();

private:
    static int callback(void *param, int argc, char **argv, char **azColName);
    static int TGTCallback(void *param, int argc, char **argv, char **azColName);
    static int TableExistCallback(void *param, int argc, char **argv, char **azColName);

private:
    sqlite3 *_db = nullptr;
};




#endif // UNIQUEAUTHDATABASE_H
