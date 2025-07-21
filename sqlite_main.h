//
// Created by iqbal on 2025-06-05.
//
#include "creditScoreAccountClass.h"
#include "db_instance.h"
#define MAX_NAME_LEN 40

double roundDec(double value, int decimal_places) {
    double power_of_ten = pow(10, decimal_places);
    return round(value * power_of_ten) / power_of_ten;
}

bool maxTransaction(double amount) {
    if (amount > 99999) {
        return false;
    } return true;
}

sqlite3* sqliteMain(){
    int rc = sqlite3_open("C:/CPP Personal Projects/BankingApp_withDB/cmake-build-debug/bank.db", &db); // open it

    const char* create_table_sql =
      "CREATE TABLE IF NOT EXISTS accounts ("
      "id INTEGER PRIMARY KEY, "
      "username TEXT NOT NULL UNIQUE, "
      "password TEXT NOT NULL, "
      "acc_num INTEGER NOT NULL UNIQUE,"
      "balance REAL DEFAULT 0.0,"
      "interestRate REAL NOT NULL DEFAULT 5, "
      "maxNoWD INTEGER NOT NULL DEFAULT 2,"
      "age INTEGER DEFAULT 0,"
      "income REAL DEFAULT 0.0,"
      "homeOwnership INTEGER DEFAULT 0);";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, create_table_sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return db;
    } else {
        std::cout << "Accounts table created (or already exists)." << std::endl;
    }
    return db;
}

bool sqliteAddAccount(const char* name0, const char* password0, int acc_num0, double balance0,
                      double interestRate0, int MAXnoWtdraws0, int age0, long income0, bool home0, sqlite3* db) {
    char buffer[512];  // Slightly larger to be safe

    snprintf(buffer, sizeof(buffer),
        "INSERT INTO accounts (username, password, acc_num, balance, interestRate, maxNoWD, age, income, homeOwnership) "
        "VALUES ('%s', '%s', %d, %.2f, %.2f, %d, %d, %ld, %d);",
        name0, password0, acc_num0, balance0, interestRate0, MAXnoWtdraws0, age0, income0, home0);

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, buffer, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "User '" << name0 << "' inserted successfully!" << std::endl;
        return true;
    }
    return false;
}

bool sqliteVerifySignIn(const char* name0, const char* password0) {
    char sql[256];
    snprintf(sql, sizeof(sql),
             "SELECT * FROM accounts WHERE username = '%s' AND password = '%s';",
             name0, password0);

    bool found = false;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
        bool* userFound = static_cast<bool*>(data);
        *userFound = true;
        return 0; // stop after first match
    };

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, callback, &found, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Login query failed: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return found;
}

CreditScoreAccount loadAccount(char* name0, char* password0) {
    const char* sql = "SELECT username, password, acc_num, balance, interestRate, maxNoWD, age, income, homeOwnership "
                      "FROM accounts WHERE username = ? AND password = ?;";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare account load.\n";
        return CreditScoreAccount(); // fallback: return a default object
    }
    sqlite3_bind_text(stmt, 1, name0, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password0, -1, SQLITE_STATIC);


    CreditScoreAccount account;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        account = CreditScoreAccount(
            (char *)sqlite3_column_text(stmt,0),
            (char *)sqlite3_column_text(stmt,1),
            sqlite3_column_int(stmt,2),
            sqlite3_column_double(stmt,3),
            sqlite3_column_double(stmt,4),
            sqlite3_column_int(stmt,5),
            sqlite3_column_int(stmt,6),
            sqlite3_column_int(stmt,7),
            sqlite3_column_int(stmt,8)
        );
    }else{
        std::cerr << "Error executing account lookup: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
    return account;
}

bool sqliteDeposit(const char* username0, double amount0) {
    const char* selectSql = "SELECT balance FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;

    double balance = 0.0;

    // Step 1: Prepare SELECT
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare account deposit.\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    } else {
        std::cerr << "Account not found: deposit failed.\n";
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);

    balance += roundDec(amount0,2);

    const char* updateSql = "UPDATE accounts SET balance = ? WHERE username = ?;";
    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare balance update (UPDATE).\n";
        return false;
    }

    sqlite3_bind_double(stmt, 1, balance);
    sqlite3_bind_text(stmt, 2, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute balance update.\n";
        return false;
    } else {
        std::cout << "$" << amount0 << " deposited into " << username0 << "'s account.\n";
        std::cout << "New balance: $" << balance << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool sqliteWithdraw(const char* username0, double amount0) {
    const char* selectSql = "SELECT balance FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;

    double balance = 0.0;

    // Step 1: Prepare SELECT
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare account withdraw.\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    } else {
        std::cerr << "Account not found: withdraw failed.\n";
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    if (balance < amount0) {
        std::cerr << "Insufficient funds.\n";
        return false;
    }
    balance -= roundDec(amount0,2);

    const char* updateSql = "UPDATE accounts SET balance = ? WHERE username = ?;";
    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare balance update.\n";
        return false;
    }

    sqlite3_bind_double(stmt, 1, balance);
    sqlite3_bind_text(stmt, 2, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute balance update.\n";
        return false;
    } else {
        std::cout << "$" << amount0 << " withdraw from " << username0 << "'s account.\n";
        std::cout << "New balance: $" << balance << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

string sqliteTransfer(const char* baseUser, const char* transferUser, double amount0) {
    const char* selectSql = "SELECT balance FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;
    sqlite3_stmt* stmt2;

    double baseBalance = 0.0;
    double transferBalance = 0.0;

    if (amount0 == NULL) {
        return "Please enter an amount";
    }

    if (strcmp(baseUser, transferUser) == 0) {
        cout << "cannot transfer to self\n";
        return "Cannot Transfer to self...";
    }

    // Step 1: Check base user
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare SELECT for base user.\n";
        return "Failed to prepare SELECT for base user.\n";
    }

    sqlite3_bind_text(stmt, 1, baseUser, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        baseBalance = sqlite3_column_double(stmt, 0);
    } else {
        std::cerr << "Base account not found.\n";
        sqlite3_finalize(stmt);
        return "Base account not found.\n";
    }
    sqlite3_finalize(stmt);

    // Step 2: Check transfer user
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt2, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare SELECT for transfer user.\n";
        return "Failed to prepare SELECT for transfer user.\n";
    }

    sqlite3_bind_text(stmt2, 1, transferUser, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt2) == SQLITE_ROW) {
        transferBalance = sqlite3_column_double(stmt2, 0);
    } else {
        std::cerr << "Recipient account not found.\n";
        sqlite3_finalize(stmt2);
        return "Recipient account not found...";
    }
    sqlite3_finalize(stmt2);

    // Step 3: Check sufficient funds
    if (baseBalance < amount0) {
        std::cerr << "Insufficient funds.\n";
        return "Insufficient funds.";
    }

    const char* updateSql = "UPDATE accounts SET balance = ? WHERE username = ?;";

    // Update base user
    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare UPDATE for base user.\n";
        return "Failed to prepare UPDATE for base user.\n";
    }

    sqlite3_bind_double(stmt, 1, baseBalance - roundDec(amount0,2));
    sqlite3_bind_text(stmt, 2, baseUser, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to update base user's balance.\n";
        sqlite3_finalize(stmt);
        return "Failed to update base user's balance.\n";
    }
    sqlite3_finalize(stmt);

    // Update transfer user
    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt2, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare UPDATE for transfer user.\n";
        return "Failed to prepare UPDATE for transfer user.\n";
    }

    sqlite3_bind_double(stmt2, 1, transferBalance + roundDec(amount0,2));
    sqlite3_bind_text(stmt2, 2, transferUser, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt2) != SQLITE_DONE) {
        std::cerr << "Failed to update transfer user's balance.\n";
        return "Failed to update transfer user's balance.\n";
    } else {
        std::cout << "$" << amount0 << " transferred from " << baseUser
                  << " to " << transferUser << ".\n";
    }
    sqlite3_finalize(stmt2);
    return "Successfully transferred " ;
}

void sqliteApplyInterest(char* username0) {
    const char* sqlquery = "SELECT balance FROM ACCOUNTS WHERE username = ?;";
    double balance = 0.0;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sqlquery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare SELECT for balance.\n";
        return;
    }

    sqlite3_bind_text(stmt, 1, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    } else {
        std::cerr << "Account not found: withdraw failed.\n";
        sqlite3_finalize(stmt);
        return;
    }

    balance *= 1.05;
    sqlite3_finalize(stmt);
    ///////////////////////////////////////////
    //modify
    const char* updateSql = "UPDATE accounts SET balance = ? WHERE username = ?;";
    sqlite3_stmt* stmt2;

    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt2, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare UPDATE for balance.\n";
        return;
    }
    sqlite3_bind_double(stmt2, 1, balance);
    sqlite3_bind_text(stmt2, 2, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt2) != SQLITE_DONE) {
        std::cerr << "Failed to execute balance update.\n";
    } else {
        std::cout << "5% interest applied to " << username0 << "'s account.\n";
        std::cout << "New balance: $" << balance << std::endl;
    }

    sqlite3_finalize(stmt2);
}

void sqliteCreateCreditAccount(const char* username0,const int age0,const double income0,const int home0) {
    const char* updateSql = "UPDATE accounts SET age = ?, income = ?, homeOwnership = ? WHERE username = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare UPDATE for balance.\n";
    }

    sqlite3_bind_text(stmt, 4, username0, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 1, age0);
    sqlite3_bind_double(stmt, 2, income0);
    sqlite3_bind_int(stmt, 3, home0);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to create credit account.\n";
    }else {
        std::cout << "Credit report generated.\n";
    }
    //account
    sqlite3_finalize(stmt);
}

void sqliteUpdateBalance(char * username0, CreditScoreAccount* account) {
    const char* sqlSelect = "SELECT balance FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;
    double balance = 0.0;

    if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare query.\n";
        return;
    }

    sqlite3_bind_text(stmt, 1, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Failed to find account.\n";
    }else {
        balance = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    account->setBalance(balance);
}

double sqliteGetBalance(const char * username0) {
    const char* sqlSelect = "SELECT balance FROM accounts WHERE username = ?;";
    sqlite3_stmt* stmt;
    double balance = 0.0;

    if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare query.\n";
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username0, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Failed to find account.\n";
    }else {
        balance = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return roundDec(balance,2);
}