#include "db-utils.h"
#include <stdio.h>

int process_entry(void *user_data, int col_num, char **col_values, char **col_names) {
    int i = 0;
    printf("%s = %s ", col_names[i], col_values[i]?col_values[i]:"NULL");
    for( i = 1; i < col_num; i++ ) {
        printf("| %s = %s ", col_names[i], col_values[i]?col_values[i]:"NULL");
    }
    printf("\n");
    return 0;
    
}

int main() {
    char *db_url = "sqlite:data/dicom.db";
    dicom_db_t dicom_db;
    dicom_query_t dicom_query;

    sql_to_sqlite_query_2("select * from test", &process_entry, &dicom_query);

    int rc = db_open(db_url, &dicom_db);
    if (rc < 0) return (-1);

    db_query(&dicom_db, &dicom_query);
    db_close(&dicom_db);
    return 0;
}
