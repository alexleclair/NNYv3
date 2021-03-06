#ifndef H_DATABASE
#define H_DATABASE

#include <iostream>
#include <sstream>
#include "ZSocket.h"
#include "mysql.h"
#include "config.hpp"
#include "pthread.h"

#define DB_USE_RESULT   false
#define DB_STORE_RESULT true
#define DB_RESULT       MYSQL_RES*
#define DB_ROW          MYSQL_ROW

class Database
{
	public:
		Database();
		~Database();
		bool        connect();
		DB_RESULT   query(bool store_result, std::string &obj);
		DB_RESULT   query(bool store_result, const char *toSend);
		DB_ROW      fetch_row(DB_RESULT result);
		void        queryDone();
		void        close();
		std::string intToStr(int number);

	private:
        MYSQL_RES       *m_sql_result;
        MYSQL           *m_mysql;
        pthread_mutex_t  m_mutex;
};

#endif
