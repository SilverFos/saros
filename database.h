#pragma once

#include<sqlite3.h>
#include<string>
#include"parse.h"

void check_error(uint8_t rc, sqlite3 *db);
int initdb(sqlite3* db, char *errorMessage_storage);
void store(const std::string database_name, const struct etherFrame* name);
