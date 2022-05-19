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
        const Username &cur_usr,
        const Username &new_usr,
        const Password &pwd,
        const Name &name,
        const MailAddr &maddr,
        int priv
    );

    int login(
        const Username &usr,
        const Password &pwd
    );

    int logout(
        const Username &usr
    );

    int query_profile(
        const Username &cur_usr,
        const Username &qry_usr
    );

    int modify_profile(
        const Username &cur_usr,
        const Username &mod_usr,
        const Password &pwd,
        const Name &name,
        const MailAddr &maddr,
        int priv
    );

    int add_train(
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
        const TrainID &id
    );

    int release_train(
        const TrainID &id
    );

    int query_train(
        const TrainID &id,
        const Date &date
    );

    template <typename Cmp>
    int query_ticket(
        const Date &date,
        const Station &start,
        const Station &term
    );

    template <typename Cmp>
    int query_transfer(
        const Date &date,
        const Station &start,
        const Station &term
    );

    int buy_ticket(
        const Username &usr,
        const TrainID &id,
        const Date &date,
        Station &start,
        Station &term,
        int num,
        bool pending_tag
    );

    int query_order(
        const Username &usr
    );

    int refund_ticket(
        const Username &usr,
        int idx
    );

    int rollback(
        int time_idx
    );

    int clean();

    int exit();

};

}

#endif