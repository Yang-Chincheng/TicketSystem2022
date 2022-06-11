#include "transaction.h"
#include <iostream>

// #define SPLIT_INTO_VOLUMES 0

namespace ticket {

int TraxManager::clear_trax() {
    rnum.clear();
    record.clear();
    pending.clear();
    // rnum.Clear();
    // record.Clear();
    // pending.Clear();    
    return 0;
}

int TraxManager::rollback_trax(int tstamp) {
    rnum.roll_back(tstamp);
    record.roll_back(tstamp);
    pending.roll_back(tstamp);   
    return 0;
}

}
