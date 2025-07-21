
// password handler
#define MAX_NAME_LEN 40

#include <iostream>
#include <string.h>
#include <cstring>
#include <ctype.h>

using namespace std;

class PasswordHandler{
private:
    string errorString;
    char pword[MAX_NAME_LEN];
public:
    bool beginValidatePassword();
    bool is8chars();
    bool hasUpperLower();
    bool hasSpecialChar();
    bool hasNumber();

    void setPassword(const char * password0) {
        strcpy(pword,password0);
    }

    string getErrorString() {
        return errorString;
    }
};

int main2() {
    char password[MAX_NAME_LEN];
    cout << ("Type at least 8 chars: ");
    cin >> password;
    PasswordHandler PHandler;
    PHandler.setPassword(password);
    if(PHandler.beginValidatePassword()){
        puts("Congratulations! Your password is valid.");
        return 0;
    }
    puts("Sorry, try again.");

}

bool PasswordHandler::is8chars(){
    return (strlen(pword) >=  8);
}

bool PasswordHandler::hasUpperLower(){
    bool hasUpper = false;
    bool hasLower = false;
    for(int i = 0; i < strlen(pword); i++){
        if(isupper(pword[i])){
            hasUpper = true;
        }else if(islower(pword[i])){
            hasLower = true;
        }
    }

    if (hasUpper && hasLower) return true;
    return false;
}

bool PasswordHandler::hasSpecialChar(){
    for(int i = 0; i < strlen(pword); i++){
        char c = pword[i];
        if(!isalnum(c) && !isspace(c)){
            return true;
        }
    } return false;
}

bool PasswordHandler::hasNumber(){
    for(int i = 0; i < strlen(pword); i++){
        char c = pword[i];
        if(isdigit(c)) {
            return true;
        }
    }return false;
}

bool PasswordHandler::beginValidatePassword(){
    if(!this->is8chars()){
        errorString =( "Error: Password must contain at least 8 characters.");
        return false;
    }

    if(!this->hasUpperLower()){
        errorString = ("Error: Password must contain at least one uppercase and one lowercase letter.");
        return false;
    }

    if(!this->hasSpecialChar()){
        errorString = ("Error: Password must contain a special character.");
        return false;
    }

    if(!this->hasNumber()){
        errorString =("Error: Password must contain a number.");
        return false;
    }
    return true;
}