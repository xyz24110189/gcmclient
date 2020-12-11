#include "uniqueAuthDatabase.h"
#include <cstdio>
#include <thread> // std::this_thread::sleep_for
#include <chrono> // std::chrono::seconds

UniqueAuthDatabase::UniqueAuthDatabase()
{
}

UniqueAuthDatabase::~UniqueAuthDatabase()
{
    sqlite3_close(_db);
}

bool UniqueAuthDatabase::InitDb()
{
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(DB_NAME, &_db);

    if (rc) {
        LOG_ERROR << "Can't open database: " << sqlite3_errmsg(_db);
        return false;
    }
    else {
        LOG_INFO << "Opened database successfully.";
        bool bRet = CreateTable();

        if (!bRet)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (!CreateTable())
            {
                sqlite3_close(_db);
                exit(-1);
            }
        }
        return true;
    }
}

bool UniqueAuthDatabase::InsertTGT(const AppData &data, const std::string &strTGT)
{
    char *zErrMsg = 0;
    int rc;

    /* Create SQL statement */
    char sql[BUFSIZ] = { 0 };
    std::snprintf(sql, BUFSIZ, "INSERT INTO TGT(APPSIGN, SECMARK, TGTDATA) VALUES('%s', '%s', '%s')", \
        data.appSign.c_str(), data.secMark.c_str(), strTGT.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(_db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        LOG_ERROR << "InsertTGT SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    else {
        LOG_INFO << "InsertTGT Operation done successfully!";
        return true;
    }
}

bool UniqueAuthDatabase::QueryLocalTGT(const AppData &data, std::string &strTGT)
{
    char *zErrMsg = 0;
    int rc;

    /* Create SQL statement */
    char sql[BUFSIZ] = { 0 };
    std::snprintf(sql, BUFSIZ, "SELECT TGTDATA from TGT WHERE APPSIGN='%s' AND SECMARK='%s'", data.appSign.c_str(), data.secMark.c_str());

    /* Execute SQL statement */
    rc = sqlite3_exec(_db, sql, TGTCallback, (void*)&strTGT, &zErrMsg);

    if (rc != SQLITE_OK) {
        LOG_ERROR << "QueryLocalTGT SQL error: ", zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    else {
        LOG_INFO << "QueryLocalTGT Operation done successfully";
        return !strTGT.empty();
    }
}

bool UniqueAuthDatabase::GetLocalTGT(std::string &strTGT)
{
    char *zErrMsg = 0;
    int rc;

    /* Create SQL statement */
    char sql[BUFSIZ] = { 0 };
    std::snprintf(sql, BUFSIZ, "SELECT TGTDATA from TGT");

    /* Execute SQL statement */
    rc = sqlite3_exec(_db, sql, TGTCallback, (void*)&strTGT, &zErrMsg);

    if (rc != SQLITE_OK) {
        LOG_ERROR << "GetLocalTGT SQL error: ", zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    else {
        LOG_INFO << "GetLocalTGT Operation done successfully";
        return !strTGT.empty();
    }
}

bool UniqueAuthDatabase::DeleteTGT()
{
    char *zErrMsg = 0;
    int rc;
    char *sql;

    /* Create SQL statement */
    sql = "DELETE FROM TGT";

    /* Execute SQL statement */
    rc = sqlite3_exec(_db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        LOG_ERROR << "DeleteTGT SQL error: ", zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    else {
        LOG_INFO << "DeleteTGT Operation Done successfully.";
        return true;
    }
}

bool UniqueAuthDatabase::CreateTable()
{
    if (TableExistJudge()) return true;

    char *zErrMsg = 0;
    int rc;

    /* Create SQL statement */
    char *sql = "CREATE TABLE TGT(" \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL," \
        "APPSIGN                         TEXT  NOT NULL," \
        "SECMARK                         TEXT  NOT NULL," \
        "TGTDATA                         TEXT  NOT NULL)";

    /* Execute SQL statement */
    rc = sqlite3_exec(_db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        LOG_ERROR << "CreateTable SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    else {
        LOG_INFO << "CreateTable Operator Done successfully.";
        return true;
    }
}

bool UniqueAuthDatabase::TableExistJudge()
{
    char *zErrMsg = 0;
    int rc;
    bool hasTable = false;

    /* Create SQL statement */
    char sql[BUFSIZ] = { 0 };
    std::snprintf(sql, BUFSIZ, "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", "TGT");

    /* Execute SQL statement */
    rc = sqlite3_exec(_db, sql, TableExistCallback, (void*)&hasTable, &zErrMsg);

    if (rc != SQLITE_OK) {
        LOG_ERROR << "TableExistJudge SQL error:" << zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    else {
        LOG_INFO << "TableExistJudge Operation done successfully";
        return hasTable;
    }
}

int UniqueAuthDatabase::callback(void *param, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++) {
        LOG_INFO << "select from table " << azColName[i] << " " << (argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

int UniqueAuthDatabase::TGTCallback(void *param, int argc, char **argv, char **azColName)
{
    std::string *strTGT = (std::string *)param;

    int i;
    for (i = 0; i < argc; i++) {
        *strTGT = argv[i];
        LOG_INFO << "select TGT from table" << azColName[i] << " " << (argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

int UniqueAuthDatabase::TableExistCallback(void *param, int argc, char **argv, char **azColName)
{
    bool *hasTable = (bool *)param;

    if (argc > 0)
        *hasTable = true;
    return 0;
}



