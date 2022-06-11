#include "transaction.h"
#include <iostream>

namespace ticket {

int TraxManager::clear_trax() {
    rnum.clear();
    record.clear();
    pending.clear();    
    return 0;
}

int TraxManager::rollback_trax(int tstamp) {
    rnum.roll_back(tstamp);
    record.roll_back(tstamp);
    pending.roll_back(tstamp);   
    return 0;
}

}
