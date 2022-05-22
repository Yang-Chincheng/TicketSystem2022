#ifndef _TICKET_SYSTEM_H_
#define _TICKET_SYSTEM_H_

#include "../lib/utility.h"
#include "../test/bptree.h"
#include "user.h"
#include "train.h"
#include "transaction.h"

namespace ticket {

struct SysManager {
private:
    UserManager user;
    TrainManager train;
    TraxManager trax;

public:
    SysManager(): user(), train(), trax() {}
    SysManager(const SysManager &o) = delete;
    
    int add_user(
        int opt_idx,
        const Username &cur_usr,
        const Username &new_usr,
        const Password &pwd,
        const Name &name,
        const MailAddr &maddr,
        int priv
    );

    int login(
        int opt_idx,
        const Username &usr,
        const Password &pwd
    );

    int logout(
        int opt_idx,
        const Username &usr
    );

    int query_profile(
        int opt_idx,
        const Username &cur_usr,
        const Username &qry_usr
    );

    int modify_profile(
        int opt_idx,
        const Username &cur_usr,
        const Username &mod_usr,
        const Password &pwd,
        const Name &name,
        const MailAddr &maddr,
        int priv
    );

    int add_train(
        int opt_idx,
        const TrainID &id,
        int station_num,
        int seat_num,
        Station *stations,
        int *prices,
        const Time &start_time,
        int *traveltimes,
        int *stoptimes,
        const Date &start_date,
        const Date &end_date,
        char type
    );

    int delete_train(
        int opt_idx,
        const TrainID &id
    );

    int release_train(
        int opt_idx,
        const TrainID &id
    );

    int query_train(
        int opt_idx,
        const TrainID &id,
        const Date &date
    );

    int query_ticket(
        int opt_idx,
        const Date &date,
        const Station &start,
        const Station &term,
        bool cmptype
    );

    int query_transfer(
        int opt_idx,
        const Date &date,
        const Station &start,
        const Station &term,
        bool cmptype
    );

    int buy_ticket(
        int opt_idx,
        const Username &usr,
        const TrainID &id,
        const Date &date,
        Station &start,
        Station &term,
        int num,
        bool pending_tag
    );

    int query_order(
        int opt_idx,
        const Username &usr
    );

    int refund_ticket(
        int opt_idx,
        const Username &usr,
        int idx
    );

    int rollback(
        int opt_idx,
        int time_idx
    );

    int clean(
        int opt_idx
    );

    int exit(
        int opt_idx
    );

};

}

#endif