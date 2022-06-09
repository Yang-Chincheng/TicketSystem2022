#include "transaction.h"
#include <iostream>

// #define SPLIT_INTO_VOLUMES 0

namespace ticket {

int TraxManager::clear_trax() {
    record.clear();
    pending.clear();
    return 0;
}

}
