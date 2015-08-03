#!/bin/bash
DB_NAME=ipvccs.db
SQL=create-db.sql
echo "Remove old db file"
rm -fr $DB_NAME
echo "Recreate db file"
sqlite3 $DB_NAME < $SQL
