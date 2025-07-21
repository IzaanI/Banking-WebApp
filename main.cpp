//
// ECE 220 Lab - Winter 2025 - Programming for Electrical Engineering
// // Lab 1: Play with a Machine
// // Lab 2: Let us Translate
// // Lab 3: Divide and Conquer
// // Lab 4: Modular and Tailored
// ---> Lab 5: Object Oriented
//
//

#include <limits>  // for std::numeric_limits
#include <vector>
#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib> // for system()
#include <random>
#include "passwordHandler.h"
#include "sqlite3.h"
#include "sqlite_main.h"
#include "baseAccountClass.h"
#include "saveBankAccountClass.h"
#include "creditScoreAccountClass.h"
#include "crow.h"
#include "crow/middlewares/cors.h"

#include "db_instance.h"
sqlite3 *db = nullptr;

#define OWN 1
#define RENT 0
#define MAX_NAME_LEN 40

using namespace std;

struct OptionsPreflightMiddleware
{
    struct context {}; // Required for middleware context

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE, PATCH");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        res.set_header("Access-Control-Max-Age", "86400"); // Cache preflight for 24 hours

        if (req.method == crow::HTTPMethod::OPTIONS)
        {
            std::cout << "DEBUG (OptionsPreflight): Handling OPTIONS preflight for: " << req.url << std::endl;
            res.code = 204; // No Content
            res.end();
            return;
        }
    }

    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/){
    }
};

crow::App<OptionsPreflightMiddleware, crow::CORSHandler> app;

void crowSignIn(){
  CROW_ROUTE(app, "/api/signin").methods("POST"_method)
  ([](const crow::request& req, crow::response& res) {
      res.set_header("Access-Control-Allow-Origin", "*");
      res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
      res.set_header("Access-Control-Allow-Headers", "Content-Type");

      auto body = crow::json::load(req.body);
      if (!body || body["username"].s() == "" || body["password"].s() == "" ){
          res.code = 400;
          res.write("{\"message\": \"Missing Username or Password.\"}");
          return res.end();
      }

      std::string user = body["username"].s();
      std::string pass = body["password"].s();

      const char * userChar = user.c_str();
      const char * passChar = pass.c_str();

      crow::json::wvalue response;

      sqlite3* activeDB = sqliteMain();
      response["message"] = (sqliteVerifySignIn(userChar,passChar))
          ? "Successfully Signed in!"
          : "Incorrect Username or Password...";
      cout << userChar << ":" << passChar << endl;
      res.code = 200;
      res.write(response.dump());
      res.end();
      sqlite3_close(activeDB);
  });
}

void crowCreateAccount(){
  CROW_ROUTE(app, "/api/createaccount").methods("POST"_method)
  ([](const crow::request& req, crow::response& res) {

    auto body = crow::json::load(req.body);
    if (!body || body["username"].s() == "" || body["password"].s() == "" ){
        res.code = 400;
        res.write("{\"message\": \"Missing Username or Password.\"}");
        return res.end();
    }

    std::string user = body["username"].s();
    std::string pass = body["password"].s();

    const char * userChar = user.c_str();
    const char * passChar = pass.c_str();

    crow::json::wvalue response;
    sqlite3* activeDB = sqliteMain();

    PasswordHandler Ph;
    Ph.setPassword(passChar);
    if (!Ph.beginValidatePassword()) {
        cout << "invalid password" << endl;
        response["message"] = Ph.getErrorString();
        res.code = 400;
        res.write(response.dump());
        res.end();
        sqlite3_close(activeDB);
        return;
    }


    std::random_device rd; // Seed
    std::mt19937 gen(rd());
    std::uniform_int_distribution < > dist(10000, 99999);

    int randAccNo = dist(gen);

    bool addAccSuccess = sqliteAddAccount(userChar, passChar, randAccNo, 0, 5, 2, 0, 0, 0, activeDB);
    response["message"] = (addAccSuccess)
    ? "Account created!"
    : "Username already taken.";
    res.write(response.dump());
    res.code = 200;
    res.end();
    sqlite3_close(activeDB);
  });
}

void crowGetBalance() {
  CROW_ROUTE(app, "/api/balance").methods("POST"_method)
  ([](const crow::request& req, crow::response& res){

    auto body = crow::json::load(req.body);
    if (!body){
      res.code = 400;
      res.write("{\"message\": \" \"}");
      return res.end();
    }

    std::string user = body["username"].s();

    crow::json::wvalue response;
    sqlite3* activeDB = sqliteMain();
    response["balance"] = sqliteGetBalance(user.c_str());
    res.code = 200;
    res.write(response.dump());
    res.end();

    sqlite3_close(activeDB);
  });
}


void crowDeposit(){
  std::cout << std::fixed;
  std::cout << std::setprecision(2);
  CROW_ROUTE(app, "/api/deposit").methods("POST"_method)
  ([](const crow::request& req, crow::response& res) {

    auto body = crow::json::load(req.body);
    if (!body || !body["deposit"]){
      res.code = 400;
      res.write("{\"message\": \"Please enter an amount.\"}");
      return res.end();
    }

    const double amount = body["deposit"].d();
    std::string user = body["username"].s();

    printf("Deposit amount: %f", amount);
    printf("username is %s", user.c_str());

    crow::json::wvalue response;
    sqlite3* activeDB = sqliteMain();
    response["message"] = sqliteDeposit(user.c_str(),amount)
        ? std::to_string(amount)
        : "Failed Deposit of ";

    response["balance"] = sqliteGetBalance(user.c_str());
    res.code = 200;
    res.write(response.dump());
    res.end();

    sqlite3_close(activeDB);
  });
}

void crowWithdraw(){
  CROW_ROUTE(app, "/api/withdraw").methods("POST"_method)
  ([](const crow::request& req, crow::response& res) {

    auto body = crow::json::load(req.body);
    if (!body || !body["withdraw"]){
      res.code = 400;
      res.write("{\"message\": \"Please enter an amount.\"}");
      return res.end();
    }

    const double amount = body["withdraw"].d();
    std::string user = body["username"].s();

    printf("withdraw amount: %f", amount);
    printf("username is %s", user.c_str());

    crow::json::wvalue response;
    sqlite3* activeDB = sqliteMain();
    response["message"] = std::to_string(amount);

    response["status"] = sqliteWithdraw(user.c_str(),amount);

    response["balance"] = sqliteGetBalance(user.c_str());
    res.code = 200;
    res.write(response.dump());
    res.end();

    sqlite3_close(activeDB);
  });
}

void crowTransfer() {
  CROW_ROUTE(app, "/api/transfer").methods("POST"_method)
  ([](const crow::request& req, crow::response& res) {

    auto body = crow::json::load(req.body);
    if (!body || !body["username"] || !body["transferUser"] || !body["amount"]){
      res.code = 400;
      res.write("{\"message\": \"Please enter an amount.\"}");
      return res.end();
    }

    string baseUser = body["username"].s();
    string transferUser = body["transferUser"].s();
    const double amount = body["amount"].d();

    crow::json::wvalue response;
    sqlite3* activeDB = sqliteMain();
    response["message"] = sqliteTransfer(baseUser.c_str(),transferUser.c_str(),amount);
    response["balance"] = sqliteGetBalance(baseUser.c_str());

    res.code = 200;
    res.write(response.dump());
    res.end();

    sqlite3_close(activeDB);

  });
}

void crowCreditCheck() {
  CROW_ROUTE(app, "/api/check-credit").methods("POST"_method)
  ([](const crow::request& req, crow::response& res) {

    auto body = crow::json::load(req.body);
    if (!body || !body["username"]){
      res.code = 400;
      res.write("{\"message\": \"Please enter an amount.\"}");
      return res.end();
    }

    string user = body["username"].s();
    const int age =body["age"].i();
    const double income= body["income"].d();
    const int home= body["home"].i();
    printf("%d %f %d", age, income, home);

    crow::json::wvalue response;
    sqlite3* activeDB = sqliteMain();

    sqliteCreateCreditAccount(user.c_str(),age,income,home);
    CreditScoreAccount Acc(user.c_str(),"pword",-1,0,5,2,age,income,home);
    response["creditScore"]= Acc.CreditScoreReport();

    res.code = 200;
    res.write(response.dump());
    res.end();

    sqlite3_close(activeDB);
  });
}

#ifdef DELETE
#undef DELETE
#endif

void runCrow() {
  app.get_middleware<crow::CORSHandler>()
      .global()
      .origin("*")
      .methods(crow::HTTPMethod::GET)
      .methods(crow::HTTPMethod::POST)
      .methods(crow::HTTPMethod::OPTIONS)
      .methods(crow::HTTPMethod::PUT)
      .methods((crow::HTTPMethod::DELETE))
      .headers("Content-Type", "Authorization", "X-Requested-With")
      .allow_credentials()
      .expose("Content-Length");

  CROW_ROUTE(app, "/")([] {
      return "Crow is running.";
  });

  CROW_ROUTE(app, "/favicon.ico")([] { return ""; });

  CROW_ROUTE(app, "/api/ping").methods("GET"_method)
  ([](const crow::request&, crow::response& res){
      crow::json::wvalue response;
      response["message"] = "pong";
      res.code = 200;
      res.write(response.dump());
      res.end();
  });

  crowSignIn();
  crowCreateAccount();
  crowGetBalance();
  crowDeposit();
  crowWithdraw();
  crowTransfer();
  crowCreditCheck();

  app.port(18080).multithreaded().run();
}



template < typename T >
  T getValidatedInput(const string & prompt, T minValue = std::numeric_limits < T > ::lowest(), T maxValue = std::numeric_limits < T > ::max()) {
    T value;
    while (true) {
      cout << prompt;
      cin >> value;

      if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits < streamsize > ::max(), '\n');
        cout << "Invalid input. Please try again.\n";
      } else if (value < minValue || value > maxValue) {
        cout << "Input must be between " << minValue << " and " << maxValue << ". Try again.\n";
      } else {
        cin.ignore(numeric_limits < streamsize > ::max(), '\n');
        return value;
      }
    }
  }

class BankingMenu {
  private: int input;
  public: void printMenu() {
    cout << "\n";
    puts("Welcome to the Banking Menu.");
    puts("1. Make a Deposit");
    puts("2. Withdraw Money");
    puts("3. Transfer Money");
    puts("4. Apply Interest");
    puts("5. Credit Report");
    puts("6. Print Summary");
    puts("7. Sign out");
  }
  int getBMenuInput();
};


class signInMenu {
  private: vector < CreditScoreAccount > accounts; // vector of created accounts
  public: void printSIMenu();
  void getMenuInput();
  void signIn();
  void createAccount();

  vector < CreditScoreAccount > & getAccounts() {
    return accounts;
  }
};

int main(void) {
  runCrow();
  return 0;
}

////**********************************************
//// class BankAccount: methods
////**********************************************

BankAccount::BankAccount() {
  name[0] = 0;
  account_number = 0;
  balance = 0.0;
}

BankAccount::BankAccount(char * n) {
  strcpy(name, n);
  account_number = 0;
  balance = 0.0;
}

BankAccount::BankAccount(char * n, int a_no, double bal) {
  strcpy(name, n);
  account_number = a_no;
  balance = bal;
}

BankAccount::BankAccount(char * uname, char * pword, int a_no, double bal) {
  strcpy(name, uname);
  strcpy(password, pword);
  account_number = a_no;
  balance = bal;
}

void BankAccount::printSummary() {
  //cout << "---------------------" << endl;
  cout << "Name: " << name << endl;
  cout << "Account Number: " << account_number << endl;
  cout << "Balance: " << balance << endl;
  cout << "---------------------" << endl;
}

double BankAccount::withdraw(double toWD) {
  if (balance - toWD > 0) {
    balance -= toWD;
    cout << "Withdraw succeed!: " << toWD << endl;
    return toWD; //teng4 modified.
  } else {
    cout << "Sorry, not enough money!!!" << endl;
    cout << "Withdraw failed!: " << toWD << endl;
    return 0;
  }
}

void BankAccount::deposit(double toDep) {
  balance += toDep;
  cout << "Deposit succeed!: " << toDep << endl;
}

double BankAccount::getBalance() {
  return balance;
}

void BankAccount::setName(char * n) {
  strcpy(name, n);
}

void BankAccount::setAccount_number(int a_no) {
  account_number = a_no;
}

void BankAccount::setBalance(double bal) {
  balance = bal;
}

////**********************************************
//// class saveBankAccount: methods
////**********************************************

// add your code here
saveBankAccount::saveBankAccount(): BankAccount() {
  interestRate = 0.0;
  MAXnoWithDraws = 0;
  noWithDraws = 0;
}
saveBankAccount::saveBankAccount(const char * name0,
  const char * password0, int account_number0, double balance0, double interestRate0, int MAXnoWithDraws0): BankAccount() {
  strcpy(name, name0);
  strcpy(password, password0);
  interestRate = interestRate0;
  MAXnoWithDraws = MAXnoWithDraws0;
  account_number = account_number0;
  balance = balance0;
  noWithDraws = 0;
}

void saveBankAccount::printSummary() {
  // method for new class that prints out the new member data
  cout << "Name: " << name << endl;
  cout << "Account Number: " << account_number << endl;
  cout << "Balance: $" << balance << endl;
  cout << "Interest Rate: " << interestRate << "%" << endl;
  cout << "---------------------" << endl;
}

void saveBankAccount::callInterest() {
  // method that performs an interest calculation and adds it to current balance
  double interest = interestRate * balance / 100;
  balance += interest;
  cout << "Interest Call Complete. \nNew Balance: $" << balance << endl;
  cout << "---------------------" << endl;
}

void saveBankAccount::resetWithdraws() {
  // method that sets withdraws to 0
  noWithDraws = 0;
  cout << "Number of Withdraws set to 0." << endl;
  cout << "---------------------" << endl;
}

void saveBankAccount::transfer(saveBankAccount & acc, double amount) {
  // method that transfers an amount if possible
  if (amount > balance) {
    cout << "Sorry! You don't have enough in your account :(" << endl;
    return;
  } else {
    balance -= amount;
    acc.balance += amount;
    cout << "You have successfully transferred $" << amount << " from " << name << "'s account to " << acc.name << "'s account!" << endl;
  }
  cout << "---------------------" << endl;
}

double saveBankAccount::withdraw(double amount) {
  // withdraw method for saveBankAccount class that only proceeds if there is enough in account
  // and withdraw limit is not exceeded
  if (amount > balance) {
    cout << "Sorry! You don't have enough in your account :(" << endl;
    return 0;
  } else if (noWithDraws >= MAXnoWithDraws) {
    cout << "Sorry! You have reached the maximum number of withdraws... (" << MAXnoWithDraws << ")" << endl;
    return 0;
  } else {
    balance -= amount;
    noWithDraws += 1;
    cout << "You have successfully withdrew $" << amount << " from " << name << "'s account!" << endl;
    return amount;
  }
}

////**********************************************
//// class CreditScoreAccount: methods
////**********************************************

CreditScoreAccount::CreditScoreAccount(const char * name0, const char * password0, int acc_num0, double balance0, double interestRate0, int MAXnoWtdraws0, int age0, long income0, bool home0): saveBankAccount() {
  // constructor that initializes an individual's credit score account with the below data
  strcpy(name, name0);
  strcpy(password, password0);
  account_number = acc_num0;
  balance = balance0;
  interestRate = interestRate0;
  MAXnoWithDraws = MAXnoWtdraws0;
  age = age0;
  income = income0;
  home_ownership = home0;
  CreditScore = 0;
}

int CreditScoreAccount::CreditScoreReport() {
  // performs an eligibility check based on age, income, and homeownership
  int age_factor, income_factor, home_factor;
  if (age < 22) age_factor = 100;
  else if (age < 27) age_factor = 120;
  else if (age < 32) age_factor = 185;
  else if (age < 39) age_factor = 200;
  else if (age < 45) age_factor = 210;
  else if (age < 50) age_factor = 225;
  else age_factor = 250;

  if (income < 10000) income_factor = 120;
  else if (income < 20000) income_factor = 140;
  else if (income < 30000) income_factor = 180;
  else if (income <50000) income_factor = 200;
  else if (income < 65000) income_factor = 225;
  else if (income < 75000) income_factor = 230;
  else income_factor = 260;

  if (home_ownership == 1) home_factor = 225;
  else home_factor = 110;

  //-----------------------------------
  CreditScore = age_factor + income_factor + home_factor;
  cout << "---------------------" << endl;
  cout << "Credit report for " << name << endl;
  cout << "Age factor: " << age_factor << endl;
  cout << "Income factor: " << income_factor << endl;
  cout << "Home ownership factor: " << home_factor << endl;
  cout << "Credit Score: " << CreditScore << endl;
  cout << "" << endl;

  if (CreditScore < 450)
    cout << "Sorry, " << name << " is not eligible!" << endl;
  else if (CreditScore < 600)
    cout <<  name << " is eligible for a standard account." << endl;
  else cout << name << " is eligible for a premium account." << endl;
  cout << "---------------------" << endl;
  return CreditScore;
}

void CreditScoreAccount::obtainCredentials() {
  puts("Please fill out the information to generate a credit report and check eligibility.");

  age = getValidatedInput < int > ("Age: ", 0, 120); // Age must be between 0 and 120
  income = getValidatedInput < double > ("Income: ", 0.0); // Income must be >= 0
  home_ownership = getValidatedInput < bool > ("Home Ownership (1 if yes, 0 if no)?: ", 0, 1); // Only 0 or 1
  creditAccountActive = true;
}

void CreditScoreAccount:: setCreditScoreAccount() {

}

////**********************************************
//// class menu: methods
////
void signInMenu::printSIMenu() {
  //cout<< "\n";
  puts("1. Sign in");
  puts("2. Create an Account");
  puts("3. Quit");
  cout << ("How can we help you? ");
}

void signInMenu::getMenuInput() {
  int choice;
  while (true) {
    cin >> choice;

    if (cin.fail()) {
      // Handle non-integer input
      cin.clear();
      cin.ignore(numeric_limits < streamsize > ::max(), '\n');
      cout << "Invalid input. Please enter a number (1-3)." << endl;
      continue;
    }

    // Handle valid input range
    switch (choice) {
    case 1:
      this -> signIn();
      break;
    case 2:
      this -> createAccount();
      cout << "\n";
      printSIMenu();
      break;
    case 3:
      cout << "Goodbye!" << endl;
      return; // exit the function
    default:
      cout << "Invalid option. Please enter 1, 2, or 3." << endl;
      break;
    }
  }
}

void signInMenu::createAccount() {
  char username[MAX_NAME_LEN], password[MAX_NAME_LEN];
  cout << "Please Create a Username and Password."
  "\nPassword must contain each of the following:"
  "\n- Uppercase letter"
  "\n- Lowercase letter"
  "\n- Number"
  "\n- Special Character"
  "\n\nUsername: ";
  cin >> username;
  cout << "Password: ";
  cin >> password;

  for (int i = 0; i < accounts.size(); i++) {
    if (strcmp(username, accounts[i].getUsername()) == 0) {
      puts("This Username already exists. Please try again.");
      return;
    }
  }

  PasswordHandler PH;
  PH.setPassword(password);
  while (!PH.beginValidatePassword()) {
    puts("Please try again.");
    cout << "Password: ";
    cin >> password;
    PH.setPassword(password);
  }

  std::random_device rd; // Seed
  std::mt19937 gen(rd()); // Mersenne Twister engine
  std::uniform_int_distribution < > dist(10000, 99999);

  int randAccNo = dist(gen);

  CreditScoreAccount newAccount(username, password, randAccNo, 0, 5, 2, 0, 0, 0);
  accounts.push_back(newAccount);

  puts("Account Created Successfully!");
}

void signInMenu::signIn() {
  int input;
  char username[MAX_NAME_LEN], password[MAX_NAME_LEN];
  cout << "Please Enter Your Username and Password.\nUsername: ";
  cin >> username;
  cout << "Password: ";
  cin >> password;

  if (sqliteVerifySignIn(username, password)) {
    printf("%s has succesfully signed in!", username);
    CreditScoreAccount account = loadAccount(username,password);
    BankingMenu BMenu;

    input = 0;
    while (input != 7) {
      BMenu.printMenu();
      input = BMenu.getBMenuInput();
      account.transactionHandler(input, accounts);
    }
    cout << "Successfully signed out.\n";
  }else {puts("Incorrect username or password.");}
  this -> printSIMenu();
}

//******************************************** / banking menu: methods /
int BankingMenu::getBMenuInput() { // needs some editing when we have all options complete
  cout << ("How can we help you? ");
  int choice;
  while (true) {
    cin >> choice;

    if (cin.fail()) {
      // Handle non-integer input
      cin.clear();
      cin.ignore(numeric_limits < streamsize > ::max(), '\n');
      cout << "Invalid input. Please enter a number (1-8)." << endl;
      return 0;
    }

    // Handle valid input range
    if (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5 && choice != 6 && choice != 7) {
      cout << "Invalid option. Please pick an option 1-7." << endl;
    }else{
      return choice;
    }
    return 0;
  }
}

void CreditScoreAccount::transactionHandler(int input, vector < CreditScoreAccount > & accounts) {
  double amount;
  string stringName;

  if (input == 1) {
    cout << "How much money would you like to deposit? ";
    cin >> amount;
    if (cin.fail() || amount <= 0) {
      cin.clear();
      cin.ignore(numeric_limits < streamsize > ::max(), '\n');
      puts("Invalid input. Please enter a positive number.");
      return;
    }
    sqliteDeposit(name, amount);
    //this -> deposit(amount);
  } else if (input == 2) {
    cout << "How much money would you like to withdraw? ";
    cin >> amount;
    if (cin.fail() || amount <= 0) {
      cin.clear();
      cin.ignore(numeric_limits < streamsize > ::max(), '\n');
      puts("Invalid input. Please enter a positive number.");
      return;
    }
    sqliteWithdraw(name, amount);
    //this -> withdraw(amount);

  } else if (input == 3) {
    char transferName[256];
    cout << "How much money would you like to transfer? ";
    cin >> amount;
    if (cin.fail() || amount <= 0) {
      cin.clear();
      cin.ignore(numeric_limits < streamsize > ::max(), '\n');
      puts("Invalid input. Please enter a positive number.");
      return;
    }
    cout << "Enter the username of the account you would like to transfer funds to: ";
    cin >> stringName;

    //convert string to type char []
    strcpy(transferName, stringName.c_str());
    sqliteTransfer(name,transferName,amount);
  } else if (input == 4) {
    sqliteApplyInterest(name);
  } else if (input == 6) {
    sqliteUpdateBalance(name, this);
    this -> printSummary();
  } else if (input == 5) {
      this -> obtainCredentials();
      sqliteCreateCreditAccount(name, age, income, home_ownership);
      this -> CreditScoreReport();
  }
}