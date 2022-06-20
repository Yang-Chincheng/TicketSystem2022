#ifndef _TICKET_SYSTEM_H_
#define _TICKET_SYSTEM_H_

#include "../lib/utility.h"
#include "../lib/cached_bptree.h"
#include "user.h"
#include "train.h"
#include "transaction.h"
#include <string>

namespace ticket {
    

class SysManager: public UserManager, public TrainManager, public TraxManager {
private:

public:
    SysManager(): UserManager(), TrainManager(), TraxManager() {}
    SysManager(const SysManager &o) = delete;

    int buy_ticket(
        unsigned long opt_idx,
        const Username &usr,
        const TrainID &id,
        const Date &date,
        Station &start,
        Station &term,
        int num,
        bool pending_tag
    );

    int query_order(
        unsigned long opt_idx,
        const Username &usr
    );

    int refund_ticket(
        unsigned long opt_idx,
        const Username &usr,
        int idx
    );

    int rollback(
        unsigned long opt_idx,
        int time_idx
    );

    int clean(
        unsigned long opt_idx
    );

    int exit(
        unsigned long opt_idx
    );

};

}

#endif