#include "comps.h"

// Comparator objects
bool AlphaWPComp::operator()(const WebPage* lhs, const WebPage* rhs)    {
    return lhs->filename() < rhs->filename();
}

bool BidComp::operator()(const Bid& lhs, const Bid& rhs)    {
    return lhs.bet > rhs.bet;
}

bool DoubleComp::operator()(const Pair lhs, const Pair rhs){
    return lhs.pr > rhs.pr;
}
