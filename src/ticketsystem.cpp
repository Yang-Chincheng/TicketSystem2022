#include "ticketsystem.h"
#include <iostream>

namespace ticket {

int SysManager::add_user(const Username &cur_usr, const Username &new_usr, const Password &pwd, const Name &name, const MailAddr &maddr, int priv) 
{
    try {user.add_user(cur_usr, new_usr, pwd, name, maddr, priv); }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::login(const Username &usr, const Password &pwd) 
{
    try {user.login(usr, pwd); }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::logout(const Username &usr) 
{
    try {user.logout(usr); }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::query_profile(const Username &cur_usr, const Username &qry_usr) 
{
    try {
        UserPack pack;
        user.query_profile(cur_usr, qry_usr, pack);
        std::cout << pack << std::endl;
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::modify_profile(const Username &cur_usr, const Username &mod_usr, const Password &pwd, const Name &name, const MailAddr &maddr, int priv) 
{
    try {
        user.modify_profile(cur_usr, mod_usr, pwd, name, maddr, priv);
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::add_train(const TrainID &id, int station_num, int seat_num, Station *stations, int *prices, const Time &start_time, int *traveltimes, int *stoptimes, const Date &start_date, const Date &end_date, char type) 
{
    try {
        train.add_train(
            id, station_num, seat_num, 
            stations, prices, start_time, traveltimes, stoptimes, 
            start_date, end_date, type
        );
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::delete_train(const TrainID &id) 
{
    try {
        train.delete_train(id);
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::release_train(const TrainID &id) 
{
    try {
        train.release_train(id);
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::query_train(const TrainID &id, const Date &date) 
{
    try {
        LinePack pack;
        train.query_train(id, date, pack);
        std::cout << pack << std::endl;
    }
    catch(exception e) {return -1; }
    return 0;
}

template <typename Cmp>
int SysManager::query_ticket(const Date &date, const Station &start, const Station &term) 
{
    try {
        TravelPack pack;
        train.query_ticket(start, term, date, pack);
        std::cout << pack << std::endl;
    }
    catch(exception e) {return -1; }
    return 0;
}

template <typename Cmp>
int SysManager::query_transfer(const Date &date, const Station &start, const Station &term) 
{
    try {
        TransPack pack;
        train.query_transfer(start, term, date, pack);
        std::cout << pack << std::endl;
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::buy_ticket(const Username &usr, const TrainID &id, const Date &date, Station &start, Station &term, int num, bool pending_tag) 
{
    try {
        TicketPack tick;
        train.check_ticket(id, date, start, term, num, tick);
        if(tick.seat >= num) {
            trax.append_record(
                usr, SUCCESS, id, start, term, 
                tick.leave, tick.arrive, num, tick.price, 
                tick.day, tick.sidx, tick.tidx
            );    
        }
        else if(pending_tag) {
            int len = trax.append_record(
                usr, PENDING, id, start, term, 
                tick.leave, tick.arrive, num, tick.price, 
                tick.day, tick.sidx, tick.tidx
            );
            trax.append_pending(
                id, tick.day, usr, len - 1, tick.sidx, tick.tidx, num
            );
        }
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::query_order(const Username &usr) 
{
    try {
        vector<TraxPack> pack;
        trax.query_record(usr, pack);
        for(TraxPack &ord: pack) {
            std::cout << ord << std::endl;
        }
    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::refund_ticket(const Username &usr, int idx) 
{
    try {
        TraxPack refnd;
        trax.query_record(usr, idx, refnd);
        trax.update_status(usr, idx, REFUNDED);
        vector<PendPack> pend;
        trax.query_pending(refnd.id, refnd.day, pend);
        vector<int> index;
        train.check_refund(refnd.id, refnd.day, refnd.sidx, refnd.tidx, refnd.number, pend, index);
        trax.flip_masking(refnd.id, refnd.day, index);
        for(int i: index) {
            trax.update_status(pend[i].user, pend[i].idx, SUCCESS);
        }
    }
    catch(exception e) {return -1; }
    return 0;
}


int SysManager::rollback(int time_idx) 
{
    try {

    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::clean() 
{
    try {

    }
    catch(exception e) {return -1; }
    return 0;
}

int SysManager::exit() 
{
    try {
        std::cout << "bye" << std::endl;
    }
    catch(exception e) {return -1; }
    return 0;
}

}