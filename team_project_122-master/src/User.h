#ifndef USER_H
#define USER_H
#include "Set.h"
#include "WebPage.h"
#include <unordered_map>
#include <map>

class User {
    public:
        User();
        User(std::string username,std::string password);
        ~User();   // destructor

        // Methods
        bool login(const std::string guess) const;
        void incrementClick(const std::string sitename);
        void generatePreferences(std::map<std::string,double>& pr);
        std::string getname() const;

    private:
        std::string name,pw;    // Username and password stored in plain text cuz no danger of hack here
        std::unordered_map<std::string,int> clicks; // Stores string of site name & number of times it has been clicked
};
#endif
