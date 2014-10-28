#include "SearchEngine.h"
#include "msort.h"
#include "comps.h"
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iostream>

//Constructors
SearchEngine::SearchEngine()    {}
SearchEngine::SearchEngine(std::string fname,std::string adfname, std::string outname) : outfile(outname)   {
    // Open file
    std::ifstream in;
    in.open(fname);
    if(in.fail()) throw std::invalid_argument("Could not open file");

    // Make the web pages; they'll be empty
    std::string filename;
    while(in >> filename)  {
        pageNames.insert(std::pair<std::string,WebPage*>(filename,new WebPage(filename)));
    }
    in.close();

    // Parse the webpages
    for(std::map<std::string,WebPage*>::iterator it = pageNames.begin(); it != pageNames.end(); ++it)  {
        it->second->parse(pageNames);
        pages.push_back(it->second);
    }

    // Get the advertisor information
    initAds(adfname);
}

// Destructor
SearchEngine::~SearchEngine()   {
    // Print the advertisor information to file
    std::ofstream out(outfile);
    if(!out.is_open()) throw std::invalid_argument("Could not open file");
    // Stored in map, so should be alphabetical by key (company)
    for(std::map<std::string,float>::iterator it = companyBids.begin(); it != companyBids.end(); ++it)  {
        out << it->first << std::endl << it->second << std::endl << std::endl;
    }

    // Delete all webpages
    for(std::vector<WebPage*>::iterator it = pages.begin(); it != pages.end(); ++it)  {
        delete *it;
    }

    // Delete all users
    for(std::map<std::string,User*>::iterator it = users.begin(); it != users.end(); ++it)  {
        delete it->second;
    }
} 

// Look up Item
WebPage* SearchEngine::lookUp(const std::string inStr) const    {
    try {
        return pageNames.at(inStr);
    }
    catch(std::exception &e) {
        throw std::invalid_argument("File does not exist");
    }
}

// Process Search
std::deque<WebPage*> SearchEngine::processSearch(std::string inStr,bool usePR,User* us) {
    // Bring input to lowercase
    std::string line;
    std::transform(inStr.begin(),inStr.end(),inStr.begin(),::tolower);

    // Get only the first word
    std::stringstream ss;
    ss << inStr;
    ss >> inStr;
    // And then the rest of the line
    getline(ss,line);

    // Add on blank space to allow last word to be processed if AND/OR
    line += "\n";

    // Get Set S
    Set<WebPage*> S;
    if(inStr == "and") S = processAND(line);
    else if(inStr == "or") S = processOR(line);
    else S = processSingle(inStr,line);

    // Generate T
    Set<WebPage*> T = generateT(S);

    // Then make adjacency matrix & generate page rank
    if(usePR)   {
        std::map<std::string,double> pr = generatePageRank(generateAdjacency(T));
        if(us != NULL) us->generatePreferences(pr);
        return MapToDeque(pr);
    }
    else return SetToDeque(T);
}

std::deque<Bid> SearchEngine::processAds(std::string inStr) {
    // Bring input to lowercase
    std::string line;
    std::transform(inStr.begin(),inStr.end(),inStr.begin(),::tolower);

    // Get only the first word
    std::stringstream ss;
    ss << inStr;
    ss >> inStr;
    // And then the rest of the line
    getline(ss,line);

    // Add on blank space to allow last word to be processed if AND/OR
    line += "\n";

    // Make set of words used
    Set<std::string> searchterms;
    if(inStr == "and" || inStr == "or") {
        std::string word;
        for(unsigned int i=0; i<line.size(); ++i)    {
            if(alphanumeric(line[i])) word += line[i];
            else    {
                // This means we're at end of the word - check for content
                if(word.size() > 0) {
                    // Put the query to lowercase and try to get; if not, make an entry if necessary
                    std::transform(word.begin(),word.end(),word.begin(),::tolower);
                    // Then add it to result (doesn't matter if already contained for insert function)
                    searchterms.insert(word);
                    // Reset word for the next word in query
                    word = "";
                }
            }
        }
    }
    else searchterms.insert(inStr);

    // Carter's search algorithm, reimplemented
    std::deque<Bid> relevantBids;
    Set<std::string> companies;
    for(std::deque<Bid>::iterator it = bids.begin(); it != bids.end(); ++it)
    {
        if(searchterms.count(it->searchTerm) == 1 && companies.count(it->company) == 0) // checks if search term included
        {
            // Should be in order
            relevantBids.push_back(*it);
            companies.insert(it->company);
        }
    }

    return relevantBids;
}

void SearchEngine::adClicked(Bid b) {
    // If company not stored yet, store it
    std::map<std::string,float>::iterator it = companyBids.find(b.company);
    if(it == companyBids.end())   {
        companyBids.insert(std::pair<std::string,float>(b.company,b.bet));
    }
    else    {
        it->second += b.bet;
    }
}

Set<WebPage*> SearchEngine::generateT(const Set<WebPage*>& S) const {
    Set<WebPage*> T(S);

    // For each entry
    for(Set<WebPage*>::const_iterator it = S.begin(); it != S.end(); ++it)    {
        // Add all its outgoing links
        Set<WebPage*> outgoing = (*it)->allOutgoingLinks();
        for(Set<WebPage*>::iterator oit = outgoing.begin(); oit != outgoing.end(); ++oit) {
            T.insert(*oit);
        }

        // Add all its incoming links
        Set<WebPage*> incoming = (*it)->allIncomingLinks();
        for(Set<WebPage*>::iterator iit = incoming.begin(); iit != incoming.end(); ++iit)   {
            T.insert(*iit);
        }
    }
    return T;
}

std::map<std::string,Set<WebPage*> > SearchEngine::generateAdjacency(const Set<WebPage*>& T) const  {
    std::map<std::string,Set<WebPage*> > adjList;   // Actually only contains the outgoing links in set T

    // Only care about intersection of outgoing and T.
    for(Set<WebPage*>::const_iterator it = T.begin(); it != T.end(); ++it)    {
        // Find what outgoing links are a member of T and add them to adjList entry
        adjList[(*it)->filename()] = T.setIntersection((*it)->allOutgoingLinks());
    }

    return adjList;
}

// Take all elements of Set and place them in sorted deque
std::deque<WebPage*> SearchEngine::SetToDeque(Set<WebPage*>& source) const  {
    std::deque<WebPage*> destination;
    // Put into unsorted deque
    for(Set<WebPage*>::iterator it = source.begin(); it != source.end(); ++it)  {
        destination.push_back(*it);
    }

    // Now sort
    AlphaWPComp comp;
    mergeSort(destination,comp);
    return destination;
}

// Generate page ranks
std::map<std::string, double> SearchEngine::generatePageRank(std::map<std::string, Set<WebPage*> > myMap){
    std::map<std::string, double> pr;
    std::map<std::string, Set<WebPage*> >::iterator it;

    //initialize pr to 1/N
    for(it=myMap.begin(); it != myMap.end(); ++it){
        pr[it->first] = 1.0/myMap.size();
    }

    //run simulation 30 times
    for(int i = 0; i < 30; ++i){
        std::map<std::string, double> temp_pr;
        std::map<std::string, double>::iterator it2;

        //initialize temp_pr to 0
        for(it2 = pr.begin(); it2 != pr.end(); ++it2){
            temp_pr[it2->first] = 0;
        }

        //loop through the Adjacency Set
        for(it = myMap.begin(); it != myMap.end(); ++it){
            std::string name = it->first;
            Set<WebPage*> s = it->second;
            Set<WebPage*>::iterator it3;

            if(s.size() != 0){
                //add computed pagerank with dampening factor
                for(it3 = s.begin(); it3 != s.end(); ++it3){
                    temp_pr[(*it3)->filename()] += ((0.85)*pr[name])/s.size();
                }

                //add rest to all pages in the map
                for(it2 = temp_pr.begin(); it2 != temp_pr.end(); ++it2){
                    it2->second += ((0.15)*pr[name])/temp_pr.size();
                }
            }else{
                //if no outgoing links, equally distribute across all nodes
                for(it2 = pr.begin(); it2 != pr.end(); ++it2){
                    temp_pr[it2->first] += pr[name]/temp_pr.size();
                }
            }
        }

        //update pr
        pr = temp_pr;
        for(it2 = pr.begin(); it2 != pr.end(); ++it2)   {
        }
    }
    return pr;
}

std::deque<WebPage*> SearchEngine::MapToDeque(std::map<std::string, double> myMap){
    std::deque<Pair> destination;

    //puts keys and values in myMap into a Pair and puts it into destination
    for(std::map<std::string, double>::iterator it = myMap.begin(); it != myMap.end(); ++it){
        Pair p;
        p.str = it->first;
        p.pr = it->second;
        destination.push_back(p);
    }

    DoubleComp comp;
    mergeSort(destination, comp);

    std::deque<WebPage*> dest;
    while(!destination.empty()){
        dest.push_back(pageNames[destination.front().str]);
        destination.pop_front();
    }

    return dest;
}

Set<WebPage*> SearchEngine::processAND(const std::string line) {
    Set<WebPage*> result,singleword;
    std::string word="";

    // Go through and generate all the queries
    for(unsigned int i=0; i<line.size(); i++)    {
        if(alphanumeric(line[i])) word += line[i];
        else    {
            // This means we're at end of the word - check for content
            if(word.size() > 0) {
                // Put the query to lowercase and try to get; if not, make an entry if necessary
                std::transform(word.begin(),word.end(),word.begin(),::tolower);
                singleword = processSingle(word,"");
                // Then add it to result
                if(result.empty()) result = singleword;
                else result = result.setIntersection(singleword);
                // Reset word for the next word in query
                word = "";
            }
        }
    }
    return result;
}

// What to do when the user issues an or command
Set<WebPage*> SearchEngine::processOR(const std::string line)  {
    Set<WebPage*> result,singleword;
    std::string word="";

    // Go through and generate all the queries
    for(unsigned int i=0; i<line.size(); i++)    {
        if(alphanumeric(line[i])) word += line[i];
        else    {
            // This means we're at end of the word - check for content
            if(word.size() > 0) {
                // Put the query to lowercase and try to get; if not, make an entry if necessary
                std::transform(word.begin(),word.end(),word.begin(),::tolower);
                singleword = processSingle(word,"");
                // Then add it to result
                if(result.empty()) result = singleword;
                else result = result.setUnion(singleword);
                // Reset word for the next word in query
                word = "";
            }
        }
    }
    return result;
}

Set<WebPage*> SearchEngine::processSingle(const std::string inStr, const std::string line)  {
    Set<WebPage*> word;

    //Check if valid input
    if(whiteSpace(line))  {
        // Get corresponding set if it exists
        try {
            word = queries.at(inStr);
        }
        // If not, generate it
        catch(std::exception &e) {
            word = generateQuery(inStr);
            queries.insert(std::pair<std::string,Set<WebPage*> >(inStr,word));
        }
    }
    return word;
}

// Generates the set given a word, and adds to map
Set<WebPage*> SearchEngine::generateQuery(const std::string query) const    {
    Set<WebPage*> containing;
    for(std::vector<WebPage*>::const_iterator it = pages.begin(); it != pages.end(); ++it)  {
        Set<std::string> words = (*it)->allWords();
        if(words.find(query) != words.end()) containing.insert(*it);
    }
    return containing;
}

// Checks if char is alphanumeric
bool SearchEngine::alphanumeric (const char ch) const {
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch<='Z') || (ch>='0' && ch<='9'));
}

// Checks if string is only whitespace
bool SearchEngine::whiteSpace (const std::string str) const {
    for(unsigned int i=0; i<str.size(); i++) {
        if (!(str[i] == ' ' || str[i] == '\t' || str[i] == '\n')) return false;
    }
    return true;
}

void SearchEngine::initAds(std::string adfname)  {
    std::ifstream ifile(adfname);
    std::string buffer; // Is a buffer
    // Deques are now a single deque<Bid> called bids stored as a variable in the class
    int numBids; 
    getline(ifile, buffer);
    numBids = atoi(buffer.c_str()); //get how many bids are in the file
    float f;
    std::string check,random;  //check and random hold what we read in from file. will be used in while loop vvvvvvv
    int k = 0; 
    //simply reading in the file that will read in the info for advertisements
    while(getline(ifile, buffer) && (k <= numBids))
    {
        int counter = 0;
        std::istringstream iss(buffer);
        random = ""; //resets strings after each line (or bid)
        std::string term = "";
        while(iss >> check)
        {
            if(counter == 0)
            {	int k = strlen(check.c_str());
                for(int ir = 0; ir < k; ir++)
                {
                    char c = tolower(check.c_str()[ir]); //makes all search terms lower case. 
                    term += c;
                }
            }
            else if (counter == 1)
            {
                std::istringstream(check) >> f; //value associated to name is pushed 
            }
            else
            {	random = check;
                while(iss >> check)
                {
                    random = random + " " + check; //random is the name of the company
                }
                break;
            }
            ++counter;
        }
        bids.push_back(Bid(term,f,random));
        ++k;
    }

    BidComp comp;
    mergeSort(bids,comp);
}

// User lookup & login
/*  If login fails or user does not exist, return NULL
    Else return pointer to user */
User* SearchEngine::userLogin(const std::string username, const std::string password)   {
    std::map<std::string,User*>::iterator it = users.find(username);
    if(it == users.end()) return NULL;
    else    {
        if(it->second->login(password)) return it->second;
        else return NULL;
    }
}

// Create a user
/* Returns false if username already taken, true otherwise */
bool SearchEngine::createUser(const std::string username, const std::string password)   {
    if(users.find(username) != users.end()) return false;
    users.insert(std::pair<std::string,User*>(username,new User(username,password)));
    return true;
}

// Bid constructor
Bid::Bid(std::string s, float b, std::string c) : searchTerm(s),bet(b),company(c) {};
