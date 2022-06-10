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

}
