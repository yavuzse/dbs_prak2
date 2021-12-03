#include <iostream>
#include <fstream>
#include <vector>
#include "db.h"

using namespace std;

int main(int argc, char *argv[]) {
    string user;
    string credential;
    string host;
    string port;
    string database;
    string commandfilepath;
    string line;

    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);

        if (arg == "-u") {
            i++;
            user = string(argv[i]);
        } else if (arg == "-c") {
            i++;
            credential = string(argv[i]);
        } else if (arg == "-h") {
            i++;
            host = string(argv[i]);
        } else if (arg == "-p") {
            i++;
            port = string(argv[i]);
        } else if (arg == "-d") {
            i++;
            database = string(argv[i]);
        } else {
            commandfilepath = arg;
        }
    }

    db_login(user, credential, host, port, database);
    ifstream commandfile(commandfilepath);
    db_begin();

    if (commandfile.is_open()) {
        while (getline(commandfile, line)) {
            vector<string> args;

            char *arg = strtok(line.data(), " ");
            while (arg != nullptr) {
                args.push_back(string(arg));
                arg = strtok(nullptr, " ");
            }
            string lastArg = args.back();
            if (lastArg[lastArg.size() - 1] == '\r') {
                lastArg = lastArg.substr(0, lastArg.size() - 1);
            }
            args.pop_back();
            args.push_back(lastArg);

            if (args[0] == "n") {
                db_drop_table("produkt");
                db_drop_table("hersteller");

                db_create_table_hersteller();
                db_create_table_produkt();
            } else if (args[0] == "ih") {
                if (db_check_hnr(args[1]) == 0) {
                    db_insert_hersteller(args[1], args[2], args[3]);
                } else if (db_check_hnr(args[1]) == 1) {
                    db_update_hersteller(args[1], args[2], args[3]);
                } else {
                    cout << "Fehler bei insert hersteller" << endl;
                }
            } else if (args[0] == "ip") {
                if (db_check_pnr(args[1]) == 0) {
                    db_insert_produkt(args[1], args[2], args[3], args[4]);
                } else if (db_check_pnr(args[1]) == 1) {
                    db_update_produkt(args[1], args[2], args[3], args[4]);
                } else {
                    cout << "Fehler bei insert produkt" << endl;
                }
            } else if (args[0] == "i") {
                if (db_check_hnr(args[1]) == 0) {
                    db_insert_hersteller(args[1], args[2], args[3]);
                } else if (db_check_pnr(args[1]) == 1) {
                    db_update_hersteller(args[1], args[2], args[3]);
                } else {
                    cout << "Fehler bei insert hersteller" << endl;
                }

                if (db_check_pnr(args[4]) == 0) {
                    db_insert_produkt(args[4], args[5], args[6], args[1]);
                } else if (db_check_pnr(args[4]) == 1) {
                    db_update_produkt(args[4], args[5], args[6], args[1]);
                } else {
                    cout << "Fehler bei insert produkt" << endl;
                }
            } else if (args[0] == "dh") {
                db_delete_hersteller(args[1]);
            } else if (args[0] == "dp") {
                db_delete_produkt(args[1]);
            } else if (args[0] == "ch") {
                int count = db_count("hersteller");
                if (count != -1) {
                    cout << "Anzahl Hersteller " << count << endl;
                }
            } else if (args[0] == "cp") {
                int count = db_count("produkt");
                if (count != -1) {
                    cout << "Anzahl Produkt " << count << endl;
                }
            }
        }
    }
    db_commit();
    db_logout();

    return 0;
}
