#include "WebPage.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

// Empty constructor
WebPage::WebPage() : rawfile(), words(), fname() {}

// Construct with just filename; checks if file exists
WebPage::WebPage(std::string filename) : fname(filename) {
    // Try to open the file to see if it exists
    std::ifstream in;
    in.open(filename);
    if(in.fail()) throw std::invalid_argument("Passed file did not open.");
    in.close();
}

// Destructor... default is fine; we don't assign anything
WebPage::~WebPage() {}

// Get the set of all words
Set<std::string> WebPage::allWords() const    {
    return words;
}

// So we can cout it or something
std::ostream& operator<<(std::ostream &os,const WebPage& page)    {
    os << page.rawfile;
    return os;
}

// Raw content processing, not just making it
void WebPage::parse(const std::map<std::string,WebPage*>& pageNames)   {
    // Open up file
    std::ifstream in;
    in.open(fname);
    if(in.fail()) throw std::invalid_argument("Passed file did not open.");

    // Store the line in a string
    std::string line;
    // These are for processing links
    std::string linkSyntax = "](", linkText;
    size_t endBracket, startBracket, endParentheses;
    // Take one line at a time
    while(getline(in,line))  {
        // Get the links and remove all the stuff around the anchor text
        endBracket = line.find(linkSyntax);
        // While we still find linkSyntax indicators
        while(endBracket != std::string::npos) {
            startBracket = line.rfind('[',endBracket);
            endParentheses = line.find(')',endBracket);
            // Did we get matches on both sides?
            if(startBracket == std::string::npos || endParentheses == std::string::npos)
                break;
            else    {
                // Get the link
                linkText = line.substr(endBracket+2,endParentheses - endBracket-2);

                // Find the corresponding page
                try {
                    // Again, should be by reference so not memory waste
                    WebPage* wp = pageNames.at(linkText);
                    // Add to link things
                    wp->inLinks.insert(this);
                    outLinks.insert(wp);
                }
                // If it doesn't work, then w/e
                catch(std::exception &e) {
                    throw std::runtime_error("Invalid page link");
                }

                // Erase Everything around the anchor text
                line.erase(endBracket,endParentheses-endBracket+1);
                line.erase(startBracket,1);
            }

            // Find the next one
            endBracket = line.find(linkSyntax);
        }

        // Add contents of each line to rawfile and add endline to line (For delimitation)
        line += "\n";
        rawfile += line;

        // Iterate through line looking for words
        std::string tempStr;
        char ch, toLower = 'a' - 'A';
        for(unsigned int i=0;i<line.size();++i)   {
            ch = line[i];
            // Check if alphanumeric
            if((ch >= '0' && ch <='9') || (ch>='a' && ch<='z')) tempStr+=ch;
            // If uppercase, add lowercase instead
            else if(ch>='A' && ch<='Z') tempStr+=(ch + toLower);
            else    {
                // Then it's word demarcation, try to insert word if word exists
                if(tempStr.size() > 0)  {
                    try {
                        words.insert(tempStr);
                    }
                    // If it's already in, don't really care
                    catch(std::exception &e) {}
                }
                tempStr = "";   // Reset tempstr
            }
        }
    }
    in.close();
}

// Access methods
Set<WebPage*> WebPage::allOutgoingLinks() const  {return outLinks;}
Set<WebPage*> WebPage::allIncomingLinks() const  {return inLinks;}
std::string WebPage::filename() const    {return fname;}
