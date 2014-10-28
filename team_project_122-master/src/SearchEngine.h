#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H
#include "WebPage.h"
#include "User.h"
#include <vector>
#include <map>
#include "Set.h"
#include <deque>

struct Bid  {
    std::string searchTerm;
    float bet;
    std::string company;
    Bid(std::string s, float b, std::string c);
};

class SearchEngine  {
    public:
        // Constructors and Destructor
        SearchEngine();
        SearchEngine(std::string fname,std::string adfname, std::string outname);
        ~SearchEngine();

        // Process a string
        std::deque<WebPage*> processSearch(std::string inStr, bool usePR, User* us);
        std::deque<Bid> processAds(std::string inStr);
        void adClicked(Bid b);

        // Get page lookup
        WebPage* lookUp(const std::string inStr) const;

        // Users
        User* userLogin(const std::string username, const std::string password);
        bool createUser(const std::string username, const std::string password);

    private:
        // Variables
        std::vector<WebPage*> pages;
        std::map<std::string,Set<WebPage*> > queries;
        std::map<std::string,WebPage*> pageNames;
        std::deque<Bid> bids;
        std::string outfile;
        std::map<std::string,float> companyBids;
        std::map<std::string,User*> users;

        // Processing searches
        // Generate Set of items with specified search terms
        Set<WebPage*> processAND(const std::string line);   // Processing for AND case
        Set<WebPage*> processOR(const std::string line);    // Processing for OR case
        Set<WebPage*> processSingle(const std::string inStr, const std::string line);   // Processing for only 1 word
        Set<WebPage*> generateQuery(const std::string query) const; // Add new search to map, so don't need to recreate later
        // Page rank applications
        Set<WebPage*> generateT(const Set<WebPage*>& S) const;  // Expand set to 1-away.
        std::map<std::string,Set<WebPage*> > generateAdjacency(const Set<WebPage*>& T) const;   // Generate adjacency list
        std::deque<WebPage*> SetToDeque(Set<WebPage*>& source) const;   // Change Set to sorted std::deque
        std::map<std::string, double> generatePageRank(std::map<std::string, Set<WebPage*> > myMap); //Generate map with pagerank value associated to filename
        std::deque<WebPage*> MapToDeque(std::map<std::string, double> myMap);

        // Advertisements
        void initAds(std::string adfname);
        // Utilities
        bool alphanumeric(const char ch) const; // Returns true iff character is alphanumeric
        bool whiteSpace(const std::string str) const;   // Returns true iff string is all whitespace

};

struct Pair{
    std::string str;
    double pr;
};

#endif
