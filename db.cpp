//
// Created by Yavuz Caylan on 17.11.21.
//
#include <libpq-fe.h>
#include <iostream>
#include "db.h"

pg_conn *db_connection;

int
db_login(const string &user, const string &credential, const string &host, const string &port, const string &database) {
    db_connection = PQsetdbLogin(host.c_str(), port.c_str(), nullptr, nullptr, database.c_str(), user.c_str(),
                                 credential.c_str());
    std::cout << host << " " << port << " " << database << " " << user << std::endl;
    return PQstatus(db_connection) == CONNECTION_OK ? 0 : -1;
}

void db_logout() {
    PQfinish(db_connection);
}

int db_begin() {
    pg_result *begin = PQexec(db_connection, "BEGIN");
    if (PQresultStatus(begin) != PGRES_COMMAND_OK) {
        PQclear(begin);
        return -1;
    }

    PQclear(begin);
    return 0;
}

int db_commit() {
    pg_result *commit = PQexec(db_connection, "COMMIT");
    if (PQresultStatus(commit) != PGRES_COMMAND_OK) {
        PQclear(commit);
        db_rollback();
        return -1;
    }

    PQclear(commit);
    return 0;
}

int db_rollback() {
    pg_result *rollback = PQexec(db_connection, "ROLLBACK");
    if (PQresultStatus(rollback) != PGRES_COMMAND_OK) {
        PQclear(rollback);
        return -1;
    }

    PQclear(rollback);
    db_logout();
    exit(1);
}

int db_create_table_hersteller() {
    string query("CREATE TABLE hersteller ( hnr varchar(4) NOT NULL PRIMARY KEY, name varchar(30), stadt varchar(30))");
    pg_result *hersteller = PQexec(db_connection, query.c_str());

    if (PQresultStatus(hersteller) != PGRES_COMMAND_OK) {
        PQclear(hersteller);
        db_rollback();
        return -1;
    }

    PQclear(hersteller);
    return 0;
}

int db_create_table_produkt() {
    string query(
            "CREATE TABLE produkt ( pnr varchar(4) NOT NULL PRIMARY KEY, name varchar(30), preis numeric(8,2), hnr varchar(4) REFERENCES hersteller(hnr) ON UPDATE CASCADE ON DELETE CASCADE)");
    pg_result *produkt = PQexec(db_connection, query.c_str());

    if (PQresultStatus(produkt) != PGRES_COMMAND_OK) {
        PQclear(produkt);
        db_rollback();
        return -1;
    }

    PQclear(produkt);
    return 0;
}

int db_drop_table(const string &tablename) {
    string query("DROP TABLE " + tablename);
    pg_result *drop = PQexec(db_connection, query.c_str());

    if (PQresultStatus(drop) != PGRES_COMMAND_OK) {
        PQclear(drop);
        return -1;
    }

    PQclear(drop);
    return 0;
}

int db_check_hnr(const string &hnr) {
    string query("SELECT count(*) FROM hersteller WHERE hnr = '" + hnr + "';");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    int count = atoi(PQgetvalue(result, 0, 0));

    PQclear(result);
    return count;
}

int db_check_pnr(const string &pnr) {
    string query("SELECT count(*) FROM produkt WHERE pnr = '" + pnr + "';");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    int count = atoi(PQgetvalue(result, 0, 0));

    PQclear(result);
    return count;
}

int db_count(const string &tablename) {
    string query("SELECT count(*) FROM " + tablename + ";");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    int count = atoi(PQgetvalue(result, 0, 0));

    PQclear(result);
    return count;
}

int db_insert_produkt(const string &pnr, const string &name, const string &preis, const string &hnr) {
    string query(
            "INSERT INTO produkt VALUES('" + pnr + "'," + "'" + name + "'," + "'" + preis + "'," + "'" + hnr + "')");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    PQclear(result);
    return 0;
}

int db_insert_hersteller(const string &hnr, const string &name, const string &stadt) {
    string query("INSERT INTO hersteller VALUES('" + hnr + "'," + "'" + name + "'," + "'" + stadt + "')");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    PQclear(result);
    return 0;
}

int db_update_produkt(const string &pnr, const string &name, const string &preis, const string &hnr) {
    string query(
            "UPDATE produkt SET name = '" + name + "', preis = '" + preis + "', hnr = '" + hnr + "' WHERE pnr = '" +
            pnr + "';");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    PQclear(result);
    return 0;
}

int db_update_hersteller(const string &hnr, const string &name, const string &stadt) {
    string query("UPDATE hersteller SET name = '" + name + "', stadt = '" + stadt + "' WHERE hnr = '" + hnr + "';");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    PQclear(result);
    return 0;
}

int db_delete_produkt(const string &pnr) {
    string query("DELETE FROM produkt WHERE pnr ='" + pnr + "';");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    PQclear(result);
    return 0;
}

int db_delete_hersteller(const string &hnr) {
    string query("DELETE FROM hersteller WHERE hnr ='" + hnr + "';");
    pg_result *result = PQexec(db_connection, query.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        PQclear(result);
        db_rollback();
        return -1;
    }

    PQclear(result);
    return 0;
}

int db_delete() {
    string produkt_query("TRUNCATE TABLE produkt;");
    string hersteller_query("TRUNCATE TABLE hersteller;");

    pg_result *produkt = PQexec(db_connection, produkt_query.c_str());

    if (PQresultStatus(produkt) != PGRES_COMMAND_OK) {
        PQclear(produkt);
        db_rollback();
        return -1;
    }

    PQclear(produkt);

    pg_result *hersteller = PQexec(db_connection, hersteller_query.c_str());

    if (PQresultStatus(hersteller) != PGRES_COMMAND_OK) {
        PQclear(hersteller);
        db_rollback();
        return -1;
    }

    PQclear(hersteller);
    return 0;
}