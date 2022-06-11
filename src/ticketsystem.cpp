#include "ticketsystem.h"
#include <iostream>

namespace ticket {

int SysManager::buy_ticket(int opt_idx, const Username &usr_str, const TrainID &id_str, const Date &date, Station &start_str, Station &term_str, int num, bool pending_tag) 
{
    size_t usr = strhasher(usr_str);
    size_t id = strhasher(id_str);
    size_t start = strhasher(start_str);
    size_t term = strhasher(term_str);
    try {
        if(!is_online(usr_str)) throw transaction_error("user need to log in first");
        
        TrainInfo tr;
        if(!train.get(id, tr) || !tr.released) {
            throw transaction_error("ticket not found");
        }
        if(num > tr.tot_seat) {
            throw transaction_error("exceed the total number of tickets");
        }
        int sidx = tr.search_station(start_str);
        if(!sidx) {
            throw transaction_error("ticket not found");
        }
        int tidx = tr.search_station(term_str, sidx);
        if(!tidx) {
            throw transaction_error("ticket not found");
        }

        Date st_date = tr.leave_time( 0, sidx).date;
        Date ed_date = tr.leave_time(-1, sidx).date;
        if(date < st_date || date > ed_date) {
            throw transaction_error("ticket not found");
        }

        int day = date - st_date;
        SeatInfo st;
        bool fb = seat.get(make_pair(id, day), st);
        int tick = st.query_seat(sidx, tidx);
        int price = tr.total_price(sidx, tidx);

        if(num > tick && !pending_tag) {
            throw transaction_error("insufficient remaining tickets");
        }
        Status status = (num <= tick? SUCCESS: PENDING);
        Time leave = tr.leave_time(day, sidx);
        Time arrive = tr.arrive_time(day, tidx);
        
        // add a record
        int len;
        if(!rnum.get(usr, len)) len = 0;
        record.put(
            getTraxID(usr, len++),
            TraxInfo(
                opt_idx, 
                status, id_str, start_str, term_str, leave, arrive, num, price, 
                day, sidx, tidx
            ),
            opt_idx, TRAX_ROLLBACK
        );
        rnum.put(usr, len, opt_idx, TRAX_ROLLBACK);

        if(status == SUCCESS) {
            // booking request succeeded
            st.modify_seat(sidx, tidx, -num);
            seat.put(make_pair(id, day), st, opt_idx, TRAIN_ROLLBACK);
            std::cout << "[" << opt_idx << "] " << 1ll * price * num << "\n";    
        }
        else {
            // booking request queueing
            pending.put(
                getPendID(id, day, opt_idx),
                PendInfo(
                    opt_idx, usr, len, sidx, tidx, num
                ),
                opt_idx, TRAX_ROLLBACK
            );
            std::cout << "[" << opt_idx << "] queue\n";
        }
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    return 0;
}

int SysManager::query_order(int opt_idx, const Username &usr_str) 
{
    size_t usr = strhasher(usr_str);
    try {
        if(!is_online(usr_str)) throw transaction_error("user need to log in first");
        auto iter = record.lower_bound(getTraxID(usr, 0));
        auto last = record.lower_bound(getTraxID(usr, opt_idx));
        if(iter == last) {
            std::cout << "[" << opt_idx << "] 0\n";
            return 0;
        }
        vector<TraxInfo> res; res.clear();
        for(; iter != last; ++iter) res.push_back(iter.getval());
        std::cout << "[" << opt_idx << "] " << res.size() << "\n";
        // sorted by transaction time (latest to earliest)
        auto it = res.end();
        do {
            --it;
            if(it->status == SUCCESS) std::cout << "[success] ";
            if(it->status == PENDING) std::cout << "[pending] ";
            if(it->status == REFUNDED) std::cout << "[refunded] ";
            std::cout << it->id << " ";
            std::cout << it->start << " " << it->leaving.date << " " << it->leaving;
            std::cout << " -> ";
            std::cout << it->termi << " " << it->arriving.date << " " << it->arriving;
            std::cout << " " << it->price << " " << it->number << "\n";
        }
        while(it != res.begin());
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    return 0;
}

int SysManager::refund_ticket(int opt_idx, const Username &usr_str, int idx) 
{
    size_t usr = strhasher(usr_str);
    try {
        if(!is_online(usr_str)) throw transaction_error("user need to log in first");
        
        int len;
        if(!rnum.get(usr, len)) len = 0;
        if(idx > len) {
            throw transaction_error("record not found");
        }

        TraxInfo rf;
        // find the idx-th lastest record of the user
        bool fb = record.get(getTraxID(usr, len - idx), rf);
        ASSERT(fb);
        size_t id = strhasher(rf.id);

        // one booking cannot be refunded twice
        if(rf.status == REFUNDED) {
            throw transaction_error("tickets have been refunded");
        }
        // refund a succeeded booking
        else if(rf.status == SUCCESS) {
            rf.status = REFUNDED;
            // update rest seat numbers
            SeatInfo st;
            fb = seat.get(make_pair(id, rf.day), st);
            ASSERT(fb);
            st.modify_seat(rf.sidx, rf.tidx, rf.number);

            // check if there are any queueing request can be granted
            auto iter = pending.lower_bound(getPendID(id, rf.day, 0));
            auto last = pending.lower_bound(getPendID(id, rf.day, opt_idx));
            if(iter != last) {
                PendInfo pd;
                TraxInfo rec;
                vector<int> suc;
                for(; iter != last; ++iter) {
                    pd = iter.getval();
                    if(st.query_seat(pd.sidx, pd.tidx) >= pd.num) {
                        suc.push_back(pd.opt_time);
                        st.modify_seat(pd.sidx, pd.tidx, -pd.num);
                        auto ID = getTraxID(pd.user, pd.idx - 1);
                        fb = record.get(ID, rec);
                        ASSERT(fb);
                        rec.status = SUCCESS;
                        record.put(ID, rec, opt_idx, TRAX_ROLLBACK);
                    }
                }
                // remove the succeeded request from the pending list
                for(int i: suc) {
                    pending.remove(getPendID(id, rf.day, i), opt_idx, TRAX_ROLLBACK);
                }
            }
            seat.put(make_pair(id, rf.day), st, opt_idx, TRAIN_ROLLBACK); 

        }
        // refund a non-succeeded booking
        else {
            rf.status = REFUNDED;
            // remove the request from the pending list
            pending.remove(getPendID(id, rf.day, rf.opt_time), opt_idx, TRAX_ROLLBACK); 
        }
        record.put(getTraxID(usr, len - idx), rf, opt_idx, TRAX_ROLLBACK);
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    std::cout << "[" << opt_idx << "] 0\n";
    return 0;
}


int SysManager::rollback(int opt_idx, int time_idx) 
{
    try {
        if(time_idx > opt_idx) {
            throw rollback_error("illegal time stamp");
        }
        rollback_user(time_idx);
        rollback_train(time_idx);
        rollback_trax(time_idx);
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    std::cout << "[" << opt_idx << "] 0\n";
    return 0;
}

int SysManager::clean(int opt_idx) 
{
    try {
        clear_user();
        clear_train();
        clear_trax();
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    std::cout << "[" << opt_idx << "] 0\n";
    return 0;
}

int SysManager::exit(int opt_idx) 
{
    std::cout << "[" << opt_idx << "] bye\n";
    return 0;
}

}