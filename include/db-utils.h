#ifndef __DB_UTILS_H__
#define __DB_UTILS_H__
#include "sqlite3.h"

enum { DB_UTILS_SQLITE, DB_UTILS_MYSQL };

typedef struct {
    sqlite3 *db;
} sqlite_db_t;

typedef struct {
    union {
        sqlite_db_t sqlite_db;
    } db;
    int type;
} dicom_db_t;

typedef int (*f_sqlite_callback_t)(void *not_used, int argc, char **argv, char **az_colname);

typedef struct {
    char *sql;
    f_sqlite_callback_t f_select;
} sqlite_query_t;

typedef struct {
    union {
        sqlite_query_t sqlite_query;
    } query;
    int type;
} dicom_query_t;

#define DB_UTILS_SQLITE_DB( dicom_db ) (dicom_db)->db.sqlite_db.db
#define DB_UTILS_SQLITE_QUERY_SQL( dicom_query ) (dicom_query)->query.sqlite_query.sql
#define DB_UTILS_SQLITE_QUERY_CB( dicom_query ) (dicom_query)->query.sqlite_query.f_select

void sql_to_sqlite_query(char *sql, dicom_query_t *query);
void sql_to_sqlite_query_2(char *sql, f_sqlite_callback_t callback, dicom_query_t *query);
int db_open(char *connect, dicom_db_t *db);
int db_query(dicom_db_t *db, dicom_query_t *query);
int db_close(dicom_db_t *db);
#endif
