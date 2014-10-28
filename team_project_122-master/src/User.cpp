#include "User.h"
#include <iostream>

// Constructors
User::User() {}
User::User(std::string username,std::string password) : name(username),pw(password) {}

// Destructor
User::~User() {}

// login method
// Can be modified to store as hash and compare like that in future
bool User::login(const std::string guess) const {
    return guess == pw;
}

// tells us that a website has been clicked
void User::incrementClick(const std::string sitename)   {
    std::unordered_map<std::string,int>::iterator it = clicks.find(sitename);
    // Not existent, then make entry
    if(it == clicks.end())  {
        clicks.insert(std::pair<std::string,int>(sitename,1));
    }
    // Increment entry
    else    {
        ++(it->second);
    }
}

// For pagerank purposes, generates the proper preference map
void User::generatePreferences(std::map<std::string,double>& pr) {
    double prefweight = 0.5;
    std::unordered_map<std::string,int>::iterator numclicks;
    double total = 0; // total number of clicks taken into account

    // Find the total
    for(std::map<std::string,double>::iterator it = pr.begin(); it != pr.end(); ++it)   {
        numclicks = clicks.find(it->first);
        if(numclicks == clicks.end()) continue;
        else total += numclicks->second;
    }

    /*  Moving forward, the formula could increase in weight depending on how many clicks have been done.
        For now though, we're boring and just doing constant weight for preferences */
    // Iterate through and update based on number of clicks if clicks > 0
    if(total != 0)  {
        for(std::map<std::string,double>::iterator it = pr.begin(); it != pr.end(); ++it)   {
            numclicks = clicks.find(it->first);
            // If havent been clicked, just reduce
            if(numclicks == clicks.end())   {
                it->second = (1.0 - prefweight) * it->second;
            }
            // Otherwise, remake
            else    {
                it->second = (prefweight * (numclicks->second)/total) + ((1.0 - prefweight) * (it->second));
            }
        }
    }
}

std::string User::getname() const   {
    return name;
}
