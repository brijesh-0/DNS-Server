#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"

static sqlite3 *db = NULL;

int init_dns_database() {
    int rc = sqlite3_open("dns.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // Create table if it doesn't exist
    const char *sql = "CREATE TABLE IF NOT EXISTS dns_records ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "domain TEXT UNIQUE NOT NULL,"
                      "ip_address TEXT NOT NULL);";
    rc = sqlite3_exec(db, sql, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // Insert sample record for example.com
    sql = "INSERT OR IGNORE INTO dns_records (domain, ip_address) VALUES ('example.com', '12.12.12.15');";
    rc = sqlite3_exec(db, sql, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    return 0;
}

int lookup_dns_record(const char *domain, char *ip_address, size_t ip_len) {
    if (!db) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT ip_address FROM dns_records WHERE domain = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, domain, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *ip = (const char *)sqlite3_column_text(stmt, 0);
        strncpy(ip_address, ip, ip_len);
        ip_address[ip_len - 1] = '\0';
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return -1; // Domain not found
}

// Ensure database is closed on program exit
__attribute__((destructor)) static void cleanup_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}