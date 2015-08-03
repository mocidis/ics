#include "db-utils.h"
#include <string.h>
#include <stdio.h>

int sqlite_select_callback_default(void *not_used, int argc, char **argv, char **az_colname) {
    int i;
    for( i = 0; i < argc; i++ ) {
        printf("%s = %s\n", az_colname[i], argv[i]?argv[i]:"NULL");
    }
    printf("\n");
    return 0;
}

void sql_to_sqlite_query(char *sql, dicom_query_t *query) {
    DB_UTILS_SQLITE_QUERY_SQL(query) = sql;
    DB_UTILS_SQLITE_QUERY_CB(query) = &sqlite_select_callback_default;
}

void sql_to_sqlite_query_2(char *sql, f_sqlite_callback_t callback, dicom_query_t *query) {
    DB_UTILS_SQLITE_QUERY_SQL(query) = sql;
    DB_UTILS_SQLITE_QUERY_CB(query) = callback;
}

int sqlite_db_open(char *connection_str, dicom_db_t *db) {
    char *error_str = 0;
    int rc;
    rc = sqlite3_open(connection_str, &(DB_UTILS_SQLITE_DB(db)));
    if( rc ) {
        fprintf(stderr, "Error in opening sqlite db: %s\n", sqlite3_errmsg(DB_UTILS_SQLITE_DB(db)));
        sqlite3_close(DB_UTILS_SQLITE_DB(db));
        return -1;
    }
    return 0;
}

int mysql_db_open(char *connection_str, dicom_db_t *db) {
    fprintf(stderr, "MysqlDB is not supported yet\n");
    return -1;
}

int db_open(char *connection_str, dicom_db_t *db) {
    int i = 0;
    int anchor = strlen(connection_str);
    int anchor1;
    for( i = 0; i < anchor; i++ ) {
        if( connection_str[i] == ':' ) break;
    }
    if ( anchor == i ) {
        fprintf(stderr, "Connection string format error\n");
        return -1; 
    }
    anchor1 = i + 1;
    if ( 0 == strncmp( "sqlite:", connection_str, anchor1) ) {
        db->type = DB_UTILS_SQLITE;
        return sqlite_db_open(&connection_str[anchor1], db);
    }
    else if ( 0 == strncmp( "mysql:", connection_str, anchor1) ) {
        db->type = DB_UTILS_MYSQL;
        return mysql_db_open(&connection_str[anchor1], db);
    }

    fprintf(stderr, "Unknown db connection type\n");
    return -1;
}

int db_query(dicom_db_t *db, dicom_query_t *query) {
    char *error_str = 0;
    int rc;
    rc = sqlite3_exec(DB_UTILS_SQLITE_DB(db), 
                      DB_UTILS_SQLITE_QUERY_SQL(query), 
                      DB_UTILS_SQLITE_QUERY_CB(query), 0, &error_str);
    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", error_str);
        sqlite3_free(error_str);
        return -1;
    }
    return 0;
}

void sqlite_db_close(dicom_db_t *db) {
    sqlite3_close(DB_UTILS_SQLITE_DB(db));
}

void mysql_db_close(dicom_db_t *db) {
    fprintf(stderr, "close:MysqlDB is not supported yet\n");
}

int db_close(dicom_db_t *db) {
    if(db->type == DB_UTILS_SQLITE) {
        sqlite_db_close(db);
        return 0;
    }
    else if (db->type == DB_UTILS_MYSQL) {
        mysql_db_close(db);
        return 0;
    }
    
    fprintf(stderr, "Unknown db connection type\n");
    return -1;
}
