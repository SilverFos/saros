#include"database.h"
#include"parse.h"
#include<cstdio>
#include<cstdlib>
#include<vector>

std::string table_format = "create table if not exists packet(id integer primary key, timestamp datetime default current_timestamp, destaddr text, srcaddr text, ethType integer, frmType text, payloadSize integer);";

std::string insert = "INSERT INTO packet(destaddr, srcaddr, ethType, frmType, payloadSize)  VALUES (?, ? , ?, ?, ?);";

void check_error(int rc, sqlite3 *db) {
        if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) // not_executed/op_not_completed/v2-indicating_no_new_row_added
        {
                fprintf(stderr, "SQLite error: %s", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
        }
}

void openDatabase(Database& DB, const std::string& dbName)
{
	int ec = sqlite3_open(dbName.c_str(), &DB.db);
	check_error(ec, DB.db);
	ec = sqlite3_prepare_v2(DB.db, insert.c_str(), insert.size(), &DB.stmt, nullptr);
	check_error(ec, DB.db);
}

void initdb(sqlite3* db)
{
	char *errMsg = nullptr;
	int return_code = sqlite3_exec(db, table_format.c_str(), nullptr, nullptr, &errMsg);
        if(return_code != SQLITE_OK){
                fprintf(stderr, "Failed to create table: %s", errMsg);
                sqlite3_free(errMsg);
		exit(0);
        }
}

//sqlite3_stmt* stmt;
//sqlite3_prepare_v2(db, insert.c_str(), insert.size(), &stmt, nullptr);
void store(const etherFrame& ether, Database& DB)
{
	char txtBuffer[50] = {};

	std::string destName = macToString(txtBuffer, sizeof(txtBuffer), ether.destAdd);
	std::string srcName = macToString(txtBuffer, sizeof(txtBuffer), ether.srcAdd);
	std::string frmType = (ether.ethType>=1536) ? "Ethernet II (DIX)" : "IEEE 802.3 Frame";

	sqlite3_bind_text(DB.stmt, 1, destName.c_str(), destName.size(), SQLITE_TRANSIENT);
	sqlite3_bind_text(DB.stmt, 2, srcName.c_str(), srcName.size(), SQLITE_TRANSIENT);
	sqlite3_bind_int(DB.stmt, 3, ether.ethType); 
	sqlite3_bind_text(DB.stmt, 4, frmType.c_str(), frmType.size(), SQLITE_TRANSIENT);
	sqlite3_bind_int(DB.stmt, 5, ether.payload.size()); 
	sqlite3_step(DB.stmt);
	sqlite3_reset(DB.stmt);
	sqlite3_clear_bindings(DB.stmt);
}

void closeDatabase(Database &DB)
{
	sqlite3_finalize(DB.stmt);
	sqlite3_close(DB.db);
}
