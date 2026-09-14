#include"database.h"
#include<cstdio>
#include<string>

std::string table_format = "create table if not exists packet(id integer primary key, timestamp datetime default current_timestamp, destaddr text, srcaddr text, ethType integer, frmType text, payloadSize integer);";

std::string insert = "INSERT INTO packet(destaddr, srcaddr, ethType, frmType, payloadSize)  VALUES (?, ? , ?, ?, ?);";

char *errMsg = nullptr;

void check_error(uint8_t rc, sqlite3 *db) {
        if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) // not_executed/op_not_completed/v2-indicating_no_new_row_added
        {
                fprintf(stderr, "SQLite error: %s", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
        }
}


int initdb(sqlite3* db)
{
	uint8_t return_code = sqlite3_exec(db, table_format.c_str(), nullptr, nullptr, &errMsg);
        if(ec != SQLITE_OK){
                fprintf(stderr, "Failed to create table: %s", errMsg);
                sqlite3_free(errMsg);
                sqlite3_close(db);
                return -1;
        }
	return 0;
}

void store(const struct etherFrame *ether)
{

}	
