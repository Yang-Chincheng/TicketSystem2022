#include "ticketsystem.h"
#include <iostream>

namespace ticket {

int SysManager::add_user(const std::string &opt_idx, const Username &cur_usr, const Username &new_usr, const Password &pwd, const Name &name, const MailAddr &maddr, int priv) 
{
    try {user.add_user(cur_usr, new_usr, pwd, name, maddr, priv); }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::login(const std::string &opt_idx, const Username &usr, const Password &pwd) 
{
    try {user.login(usr, pwd); }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::logout(const std::string &opt_idx, const Username &usr) 
{
    try {user.logout(usr); }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::query_profile(const std::string &opt_idx, const Username &cur_usr, const Username &qry_usr) 
{
    try {
        UserPack pack;
        user.query_profile(cur_usr, qry_usr, pack);
        std::cout << opt_idx << " " << pack << std::endl;
    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::modify_profile(const std::string &opt_idx, const Username &cur_usr, const Username &mod_usr, const Password &pwd, const Name &name, const MailAddr &maddr, int priv) 
{
    try {
        user.modify_profile(cur_usr, mod_usr, pwd, name, maddr, priv);
    }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::add_train(const std::string &opt_idx, const TrainID &id, int station_num, int seat_num, Station *stations, int *prices, const Time &start_time, int *traveltimes, int *stoptimes, const Date &start_date, const Date &end_date, char type) 
{
    try {
        train.add_train(
            id, station_num, seat_num, 
            stations, prices, start_time, traveltimes, stoptimes, 
            start_date, end_date, type
        );
    }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::delete_train(const std::string &opt_idx, const TrainID &id) 
{
    try {
        train.delete_train(id);
    }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::release_train(const std::string &opt_idx, const TrainID &id) 
{
    try {
        train.release_train(id);
    }
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::query_train(const std::string &opt_idx, const TrainID &id, const Date &date) 
{
    try {
        LinePack pack;
        train.query_train(id, date, pack);
        std::cout << opt_idx << " " << pack << std::endl;
    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::query_ticket(const std::string &opt_idx, const Date &date, const Station &start, const Station &term, bool cmp_type) 
{
    try {
        vector<TravelPack> pack;
        train.query_ticket(start, term, date, cmp_type, pack);
        std::cout << opt_idx << " " << pack.size() << std::endl;
        for(TravelPack &tick: pack) std::cout << tick << std::endl;
    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::query_transfer(const std::string &opt_idx, const Date &date, const Station &start, const Station &term, bool cmp_type) 
{
    try {
        TransPack pack;
        if(!~train.query_transfer(start, term, date, cmp_type, pack)) std::cout << opt_idx << " 0" << std::endl;
        else std::cout << opt_idx << " " << pack.first << std::endl << pack.second << std::endl;
    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::buy_ticket(const std::string &opt_idx, const Username &usr, const TrainID &id, const Date &date, Station &start, Station &term, int num, bool pending_tag) 
{
    try {
        if(!user.is_online(usr)) throw transaction_error("user need to log in first");
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
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}

int SysManager::query_order(const std::string &opt_idx, const Username &usr) 
{
    try {
        if(!user.is_online(usr)) throw transaction_error("user need to log in first");
        vector<TraxPack> pack;
        trax.query_record(usr, pack);
        std::cout << opt_idx << " " << pack.size() << std::endl;
        for(TraxPack &ord: pack) {
            std::cout << "[" << opt_idx << "] " << ord << std::endl;
        }
    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::refund_ticket(const std::string &opt_idx, const Username &usr, int idx) 
{
    try {
        if(!user.is_online(usr)) throw transaction_error("user need to log in first");
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
    catch(exception e) {throw e; }
    std::cout << opt_idx << " 0" << std::endl;
    return 0;
}


int SysManager::rollback(const std::string &opt_idx, int time_idx) 
{
    try {

    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::clean(const std::string &opt_idx) 
{
    try {
        user.clear();
        train.clear();
        trax.clear();
    }
    catch(exception e) {throw e; }
    return 0;
}

int SysManager::exit(const std::string &opt_idx) 
{
    std::cout << opt_idx << " bye" << std::endl;
    return 0;
}

}