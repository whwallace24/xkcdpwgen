#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <zconf.h>

long getNumOfWords(sqlite3 *db) {
    long out = -1;
    std::string arg = "select count(*) from entries;";
    sqlite3_stmt* stmt = nullptr;
    const char *zErrMsg = nullptr;
    int rc = sqlite3_prepare(db, arg.c_str(), (int)arg.size(), &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        zErrMsg = sqlite3_errmsg(db);
        printf("%s", zErrMsg);
        return rc;
    }

    //execute the SQL query
    rc = sqlite3_step(stmt);
    if(SQLITE_ROW == rc)
    {
        out = sqlite3_column_int(stmt, 0);
    }

    //TODO - read docs to see if sqlite3_finalize ought be called here

    return out;
}

std::string SplitFilename (const std::string& str)
{
    size_t found;
    found=str.find_last_of("/\\");
    std::string ret = str.substr(0, found);
    return ret;
}

int main(int argc, char *argv[]) {
    long words = 4;
    long caps = 0;
    long nums = 0;
    long symbs = 0;
    const char *zErrMsg = nullptr;
    sqlite3 *db = nullptr;
    sqlite3_stmt* stmt = nullptr;
    std::vector <std::string> arr = {};
    std::vector <std::string> symbols = {"!", "@", "#", "$", "%", "^", "&", "*", "~", ".", ":", ";"};

    //TODO - make a separate test project and play with Boost program_options
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (strncmp(argv[i], "-w", 2) == 0 or strncmp(argv[i], "--words", 7) == 0) {
                //TODO could do argv[i++] and avoid the separate i++ line (here and below)
                std::string val = argv[i+1];
                if (val.find_first_of("0123456789") != std::string::npos) {
                    words = strtol(argv[++i], nullptr, 10);
                }
                else {
                    std::cout << "Parameters formatted incorrectly.\n"
                                 "usage: xkcdpwgen [-h] [-w WORDS] [-c CAPS] [-n NUMBERS] [-s SYMBOLS]\n"
                                 "                \n"
                                 "Generate a secure, memorable password using the XKCD method\n"
                                 "                \n"
                                 "optional arguments:\n"
                                 "    -h, --help            show this help message and exit\n"
                                 "    -w WORDS, --words WORDS\n"
                                 "                          include WORDS words in the password (default=4)\n"
                                 "    -c CAPS, --caps CAPS  capitalize the first letter of CAPS random words\n"
                                 "                          (default=0)\n"
                                 "    -n NUMBERS, --numbers NUMBERS\n"
                                 "                          insert NUMBERS random numbers in the password\n"
                                 "                          (default=0)\n"
                                 "    -s SYMBOLS, --symbols SYMBOLS\n"
                                 "                          insert SYMBOLS random symbols in the password\n"
                                 "                          (default=0)" << "\n";
                    return 1;
                }
            }
            else if (strncmp(argv[i], "-c", 2) == 0 or strncmp(argv[i], "--caps", 6) == 0) {
                std::string val = argv[i+1];
                if (val.find_first_of("0123456789") != std::string::npos) {
                    caps = strtol(argv[++i], nullptr, 10);
                }
                else {
                    std::cout << "Parameters formatted incorrectly.\n"
                                 "usage: xkcdpwgen [-h] [-w WORDS] [-c CAPS] [-n NUMBERS] [-s SYMBOLS]\n"
                                 "                \n"
                                 "Generate a secure, memorable password using the XKCD method\n"
                                 "                \n"
                                 "optional arguments:\n"
                                 "    -h, --help            show this help message and exit\n"
                                 "    -w WORDS, --words WORDS\n"
                                 "                          include WORDS words in the password (default=4)\n"
                                 "    -c CAPS, --caps CAPS  capitalize the first letter of CAPS random words\n"
                                 "                          (default=0)\n"
                                 "    -n NUMBERS, --numbers NUMBERS\n"
                                 "                          insert NUMBERS random numbers in the password\n"
                                 "                          (default=0)\n"
                                 "    -s SYMBOLS, --symbols SYMBOLS\n"
                                 "                          insert SYMBOLS random symbols in the password\n"
                                 "                          (default=0)" << "\n";
                    return 1;
                }
            }
            else if (strncmp(argv[i], "-n", 2) == 0 or strncmp(argv[i], "--numbers", 9) == 0) {
                std::string val = argv[i+1];
                if (val.find_first_of("0123456789") != std::string::npos) {
                    nums = strtol(argv[++i], nullptr, 10);
                }
                else {
                    std::cout << "Parameters formatted incorrectly.\n"
                                 "usage: xkcdpwgen [-h] [-w WORDS] [-c CAPS] [-n NUMBERS] [-s SYMBOLS]\n"
                                 "                \n"
                                 "Generate a secure, memorable password using the XKCD method\n"
                                 "                \n"
                                 "optional arguments:\n"
                                 "    -h, --help            show this help message and exit\n"
                                 "    -w WORDS, --words WORDS\n"
                                 "                          include WORDS words in the password (default=4)\n"
                                 "    -c CAPS, --caps CAPS  capitalize the first letter of CAPS random words\n"
                                 "                          (default=0)\n"
                                 "    -n NUMBERS, --numbers NUMBERS\n"
                                 "                          insert NUMBERS random numbers in the password\n"
                                 "                          (default=0)\n"
                                 "    -s SYMBOLS, --symbols SYMBOLS\n"
                                 "                          insert SYMBOLS random symbols in the password\n"
                                 "                          (default=0)" << "\n";
                    return 1;
                }
            }
            else if (strncmp(argv[i], "-s", 2) == 0 or strncmp(argv[i], "--symbols", 9) == 0) {
                std::string val = argv[i+1];
                if (val.find_first_of("0123456789") != std::string::npos) {
                    symbs = strtol(argv[++i], nullptr, 10);
                }
                else {
                    std::cout << "Parameters formatted incorrectly.\n"
                                 "usage: xkcdpwgen [-h] [-w WORDS] [-c CAPS] [-n NUMBERS] [-s SYMBOLS]\n"
                                 "                \n"
                                 "Generate a secure, memorable password using the XKCD method\n"
                                 "                \n"
                                 "optional arguments:\n"
                                 "    -h, --help            show this help message and exit\n"
                                 "    -w WORDS, --words WORDS\n"
                                 "                          include WORDS words in the password (default=4)\n"
                                 "    -c CAPS, --caps CAPS  capitalize the first letter of CAPS random words\n"
                                 "                          (default=0)\n"
                                 "    -n NUMBERS, --numbers NUMBERS\n"
                                 "                          insert NUMBERS random numbers in the password\n"
                                 "                          (default=0)\n"
                                 "    -s SYMBOLS, --symbols SYMBOLS\n"
                                 "                          insert SYMBOLS random symbols in the password\n"
                                 "                          (default=0)" << "\n";
                    return 1;
                }
            }
            else if (strncmp(argv[i], "-h", 2) == 0 or strncmp(argv[i], "--help", 6) == 0) {
                std::cout << "usage: xkcdpwgen [-h] [-w WORDS] [-c CAPS] [-n NUMBERS] [-s SYMBOLS]\n"
                             "                \n"
                             "Generate a secure, memorable password using the XKCD method\n"
                             "                \n"
                             "optional arguments:\n"
                             "    -h, --help            show this help message and exit\n"
                             "    -w WORDS, --words WORDS\n"
                             "                          include WORDS words in the password (default=4)\n"
                             "    -c CAPS, --caps CAPS  capitalize the first letter of CAPS random words\n"
                             "                          (default=0)\n"
                             "    -n NUMBERS, --numbers NUMBERS\n"
                             "                          insert NUMBERS random numbers in the password\n"
                             "                          (default=0)\n"
                             "    -s SYMBOLS, --symbols SYMBOLS\n"
                             "                          insert SYMBOLS random symbols in the password\n"
                             "                          (default=0)" << "\n";
                return 0;
            }
            else {
                std::cout << "Parameters formatted incorrectly.\n"
                             "usage: xkcdpwgen [-h] [-w WORDS] [-c CAPS] [-n NUMBERS] [-s SYMBOLS]\n"
                             "                \n"
                             "Generate a secure, memorable password using the XKCD method\n"
                             "                \n"
                             "optional arguments:\n"
                             "    -h, --help            show this help message and exit\n"
                             "    -w WORDS, --words WORDS\n"
                             "                          include WORDS words in the password (default=4)\n"
                             "    -c CAPS, --caps CAPS  capitalize the first letter of CAPS random words\n"
                             "                          (default=0)\n"
                             "    -n NUMBERS, --numbers NUMBERS\n"
                             "                          insert NUMBERS random numbers in the password\n"
                             "                          (default=0)\n"
                             "    -s SYMBOLS, --symbols SYMBOLS\n"
                             "                          insert SYMBOLS random symbols in the password\n"
                             "                          (default=0)" << "\n";
                return 1;
            }
        }
    }

    srandom((unsigned int)time(nullptr));
    for (int i = 1; i <= words; i++) {
        std::string answer;
        char buffer[answer.max_size()];
        getcwd(buffer, sizeof(buffer));
        answer = SplitFilename(buffer);
        answer.append("/Dictionary.db");
        if (sqlite3_open(answer.c_str(), &db) != SQLITE_OK) {
            printf("ERROR opening SQLite DB in memory: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("database closed.\n");
        }
        long max = getNumOfWords(db);
        long row = random() % max;

        std::ostringstream bar;
        bar << "select word from entries where rowid=?;";
        //TODO bind rowid to the query

        const unsigned char* out;
        std::string query = bar.str();
        int rc = sqlite3_prepare(db, query.c_str(), (int)query.length(), &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_finalize(stmt);
            zErrMsg = sqlite3_errmsg(db);
            printf("%s", zErrMsg);
            return rc;
        }
        sqlite3_bind_int64(stmt, 1, row);

        //execute the SQL query
        rc = sqlite3_step(stmt);
        if(SQLITE_ROW == rc)
        {
            out = sqlite3_column_text(stmt, 0);

            //TODO consider tokening the value and eliminating the undesired values
            std::string string((const char*) out);
            if ((string.find('-') != std::string::npos) or (string.find(' ') != std::string::npos)) {
                i -= 1;
            }
            else {
                std::transform(string.begin(), string.end(), string.begin(), ::tolower);
                arr.push_back(string);
            }
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    std::string password;
    while (nums >= words) {
        //use 9 because we are using 0-9 for numbers in our password
        long num = random() % 10;
        std::string number = std::to_string(num);
        password.append(number);
        nums--;
    }
    for (std::string str : arr) {
        if (caps > 0) {
            long c = std::toupper(str[0]);
            str[0] = (char)c;
            password.append(str);
            caps--;
        }
        else {
            password.append(str);
        }
        if (symbs > 0) {
            long sym = random() % symbols.size();
            password.append(symbols[sym]);
            symbs -= 1;
        }
        if (nums > 0) {
            long num = random() % 9;
            std::string number = std::to_string(num);
            password.append(number);
            nums--;
        }
    }
    while (symbs > 0) {
        long sym = random() % symbols.size();
        password.append(symbols[sym]);
        symbs -= 1;
    }
    printf("%s\n", password.c_str());
    return 0;
}