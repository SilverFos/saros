#pragma once

#include<sqlite3.h>
#include<string>
#include"parse.h"

//sqlite3* db;

struct Database{
	sqlite3* db = nullptr;
	sqlite3_stmt* stmt = nullptr;
};

void initdb(sqlite3* db);
void check_error(int rc, sqlite3* db);
void openDatabase(Database& DB, const std::string& databaseName);
void closeDatabase(Database& DB);
void store(const etherFrame& frame, Database& DB); 
