#ifndef COMPS_H
#define COMPS_H

#include "WebPage.h"
#include "SearchEngine.h"

// Relevant comparators
struct AlphaWPComp  {
    bool operator()(const WebPage* lhs, const WebPage* rhs);
};

struct BidComp  {
    bool operator()(const Bid& lhs, const Bid& rhs);
};

struct DoubleComp  {
    bool operator()(const Pair lhs, const Pair rhs);
};

#endif
