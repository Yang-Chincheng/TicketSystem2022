#include "ticketsystem.h"
#include <iostream>

namespace ticket {


int SysManager::buy_ticket(int opt_idx, const Username &usr, const TrainID &id, const Date &date, Station &start, Station &term, int num, bool pending_tag) 
{
    try {
        if(!is_online(usr)) throw transaction_error("user need to log in first");
        
        TrainInfo tr;
        if(!train.get(id, tr) || !tr.released) {
            throw transaction_error("ticket not found");
        }
        if(num > tr.tot_seat) {
            throw transaction_error("exceed the total number of tickets");
        }
        int sidx = tr.search_station(start);
        if(!sidx) {
            throw transaction_error("ticket not found");
        }
        int tidx = tr.search_station(term, sidx);
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
        
        int len;
        if(!rnum.get(usr, len)) len = 0;
        record.put(
            getTraxID(usr, len++),
            TraxInfo(
                opt_idx, 
                status, id, start, term, leave, arrive, num, price, 
                day, sidx, tidx
            )
        );
        rnum.put(usr, len);

        if(status == SUCCESS) {
            // buy ticket succeeded
            st.modify_seat(sidx, tidx, -num);
            seat.put(make_pair(id, day), st);
            std::cout << "[" << opt_idx << "] " << 1ll * price * num << std::endl;    
        }
        else {
            // buy ticket pending
            pending.put(
                getPendID(id, day, opt_idx),
                PendInfo(
                    opt_idx, usr, len, sidx, tidx, num
                )
            );
            std::cout << "[" << opt_idx << "] queue" << std::endl;
        }
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    return 0;
}

int SysManager::query_order(int opt_idx, const Username &usr) 
{
    try {
        if(!is_online(usr)) throw transaction_error("user need to log in first");
        auto iter = record.lower_bound(getTraxID(usr, 0));
        auto last = record.lower_bound(getTraxID(usr, opt_idx));
        if(iter == last) {
            std::cout << "[" << opt_idx << "] 0" << std::endl;
            return 0;
        }
        vector<TraxInfo> res; res.clear();
        for(; iter != last; ++iter) res.push_back(*iter);
        std::cout << "[" << opt_idx << "] " << res.size() << std::endl;
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
            std::cout << " " << it->price << " " << it->number << std::endl;
        }
        while(it != res.begin());
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    return 0;
}

int SysManager::refund_ticket(int opt_idx, const Username &usr, int idx) 
{
    try {
        if(!is_online(usr)) throw transaction_error("user need to log in first");
        
        int len;
        bool fb = rnum.get(usr, len);
        ASSERT(fb);
        if(idx > len) {
            throw transaction_error("record not found");
        }

        TraxInfo rf;
        fb = record.get(getTraxID(usr, len - idx), rf);
        ASSERT(fb);
        
        if(rf.status == REFUNDED) {
            throw transaction_error("tickets have been refunded");
        }
        else if(rf.status == SUCCESS) {
            rf.status = REFUNDED;
            SeatInfo st;
            fb = seat.get(make_pair(rf.id, rf.day), st);
            ASSERT(fb);
            st.modify_seat(rf.sidx, rf.tidx, rf.number);

            auto iter = pending.lower_bound(getPendID(rf.id, rf.day, 0));
            auto last = pending.lower_bound(getPendID(rf.id, rf.day, opt_idx));
            if(iter != last) {
                PendInfo pd;
                TraxInfo rec;
                vector<int> suc;
                for(; iter != last; ++iter) {
                    pd = *iter;
                    if(st.query_seat(pd.sidx, pd.tidx) >= pd.num) {
                        suc.push_back(pd.opt_time);
                        st.modify_seat(pd.sidx, pd.tidx, -pd.num);
                        auto ID = getTraxID(pd.user, pd.idx - 1);
                        fb = record.get(ID, rec);
                        ASSERT(fb);
                        rec.status = SUCCESS;
                        record.put(ID, rec);
                    }
                }
                for(int i: suc) {
                    pending.remove(getPendID(rf.id, rf.day, i));
                }
            }
            seat.put(make_pair(rf.id, rf.day), st); 
        }
        else {
            rf.status = REFUNDED;
            pending.remove(getPendID(rf.id, rf.day, rf.opt_time));            
        }
        record.put(getTraxID(usr, len - idx), rf);
    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
    std::cout << "[" << opt_idx << "] 0" << std::endl;
    return 0;
}


int SysManager::rollback(int opt_idx, int time_idx) 
{
    try {

    }
    catch(exception &e) {throw e; }
    catch(std::string &msg) {throw msg; }
    catch(...) {throw; }
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
    std::cout << "[" << opt_idx << "] 0" << std::endl;
    return 0;
}

int SysManager::exit(int opt_idx) 
{
    std::cout << "[" << opt_idx << "] bye" << std::endl;
    return 0;
}

}