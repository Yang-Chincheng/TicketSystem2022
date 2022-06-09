#ifndef _TICKET_SYSTEM_TRAIN_H_
#define _TICKET_SYSTEM_TRAIN_H_

#include "../lib/utility.h"
#include "../lib/cached_bptree.h"
#include "../lib/hashmap.h"
#include "../lib/vector.h"
#include <iostream>

namespace ticket {

// Manager
class TrainManager;

// Infomation Preserver
struct TrainInfo;
struct SeatInfo;
struct PassInfo;

// Infomation Transmitter
struct LinePack;
struct TravelPack;
using TransPack = pair<TravelPack, TravelPack>;
struct TicketPack;

const int max_stanum = 102;
const int max_date = 100;
const int max_pnum = 3000;

struct TrainInfo {
    bool released;
    char type;
    int sta_num; // 1-base
    int day_num; // 0-base
    Time st_time;
    Date st_date, ed_date;
    int tot_seat;
    
    Station sta[max_stanum];
    int price[max_stanum];
    int arri_time[max_stanum];
    int leav_time[max_stanum];
    
    TrainInfo() = default;
    TrainInfo(
        int _stanum,  
        int _seatnum,
        Station *_sta,
        int *_price,
        const Time &_st_time,
        int *_tra_time,
        int *_stp_time,
        const Date &_st_date,
        const Date &_ed_date,
        char _type
    );
    TrainInfo(const TrainInfo &o) = default;
    ~TrainInfo() = default;

    Time arrive_time(int day, int idx);
    Time leave_time(int day, int idx);
    int total_price(int s, int t);
    int search_station(const Station &sta, int st = 1, int ed = -1);

};

struct SeatInfo {
    int num;
    int seat[max_stanum];

    SeatInfo() = default;
    SeatInfo(const SeatInfo &o) = default;
    SeatInfo(int _num, int _seat): num(_num) {
        for(int i = 1; i < num; ++i) seat[i] = _seat;
    }

    int query_seat(int s, int t);
    int modify_seat(int s, int t, int del);
};

struct PassInfo {
    Date st_date;
    Date ed_date;
    TrainID id;
    int idx;
    int hint;

    PassInfo() = default;
    PassInfo(const PassInfo &o) = default;
    PassInfo(
        const Date &_st,
        const Date &_ed,
        const TrainID &_id,
        int _idx,
        int _hint
    ):
    st_date(_st), ed_date(_ed), id(_id), idx(_idx), hint(_hint) {}

};

struct TravelPack: public InfoPack {
    TrainID id;
    Station strt, term;
    Time leav, arri;
    int price;
    int seat;

    TravelPack() = default;
    TravelPack(
        const TrainID &_id,
        const Station &_strt, 
        const Station &_term, 
        const Time &_leav, 
        const Time &_arri,
        int _price, 
        int _seat
    ): 
    id(_id), strt(_strt), term(_term), 
    leav(_leav), arri(_arri), 
    price(_price), seat(_seat) {}

    friend std::ostream& operator << (std::ostream &os, const TravelPack &pack);
};

struct TicketPack: public InfoPack {
    int day, sidx, tidx;
    int price, seat;
    Time leave, arrive;

    TicketPack() = default;
    TicketPack(const TicketPack &o) = default;
    TicketPack(
        int _day,
        int _sidx,
        int _tidx,
        int _price,
        int _seat,
        const Time &_leave,
        const Time &_arrive
    ):
    day(_day), sidx(_sidx), tidx(_tidx),
    price(_price), seat(_seat),
    leave(_leave), arrive(_arrive) {}
};

struct ByTime {
    // query_ticket: answer sorting
    bool operator () (const TravelPack &lhs, const TravelPack &rhs) const {
        int t1 = lhs.arri - lhs.leav;
        int t2 = rhs.arri - rhs.leav;
        if(t1 != t2) return t1 < t2;
        return lhs.id < rhs.id;
    }
    // query_transfer: answer comparison
    bool operator () (const TransPack &lhs, const TransPack &rhs) const {
        int t1 = lhs.second.arri - lhs.first.leav;
        int t2 = rhs.second.arri - rhs.first.leav;
        if(t1 != t2) return t1 < t2;
        int p1 = lhs.first.price + lhs.second.price;
        int p2 = rhs.first.price + rhs.second.price;
        if(p1 != p2) return p1 < p2;
        if(lhs.first.id != rhs.first.id) return lhs.first.id < rhs.first.id;
        return lhs.second.id < rhs.second.id;
    }
};

struct ByCost {
    // query_ticket: answer sorting
    bool operator () (const TravelPack &lhs, const TravelPack &rhs) const {
        if(lhs.price != rhs.price) return lhs.price < rhs.price;
        return lhs.id < rhs.id;
    }
    // query_transfer: answer comparison
    bool operator () (const TransPack &lhs, const TransPack &rhs) const {
        int p1 = lhs.first.price + lhs.second.price;
        int p2 = rhs.first.price + rhs.second.price;
        if(p1 != p2) return p1 < p2;
        int t1 = lhs.second.arri - lhs.first.leav;
        int t2 = rhs.second.arri - rhs.first.leav;
        if(t1 != t2) return t1 < t2;
        if(lhs.first.id != rhs.first.id) return lhs.first.id < rhs.first.id;
        return lhs.second.id < rhs.second.id;
    }    
};

class TrainManager {
protected:
    cached_bptree<TrainID, TrainInfo, StrHasher> train;
    cached_bptree<pair<TrainID, int>, SeatInfo, PairHasher<TrainID, int, StrHasher>> seat;
    cached_bptree<pair<Station, int>, PassInfo, PairHasher<Station, int, StrHasher>> pass;

    int clear_train();

public:
    TrainManager(): train("train"), seat("seat"), pass("pass") {}
    TrainManager(const TrainManager &o) = delete;
    ~TrainManager() = default;

    int add_train(
        int opt_idx, 
        const TrainID &id,
        int _stanum,  
        int _seatnum,
        Station *_sta,
        int *_price,
        const Time &_st_time,
        int *_tra_time,
        int *_stp_time,
        const Date &_st_date,
        const Date &_ed_date,
        char _type
    );

    int delete_train(
        int opt_idx,
        const TrainID& id
    );

    int release_train(
        int opt_idx, 
        const TrainID &id
    );
    
    int query_train(
        int opt_idx,
        const TrainID& id, 
        const Date &date
    );

    int query_ticket(
        int opt_idx,
        const Station &strt,
        const Station &term,
        const Date &date,
        bool cmp_type
    );

    int query_transfer(
        int opt_idx,
        const Station &strt,
        const Station &term,
        const Date &date,
        bool cmp_type
    );

    // int check_ticket(
    //     int opt_idx,
    //     const TrainID &id,
    //     const Date &date, 
    //     const Station &strt,
    //     const Station &term,
    //     int num,
    //     TicketPack &pack
    // );

    // int check_refund(
    //     int opt_idx,
    //     bool suc,
    //     const TrainID &id,
    //     int day, 
    //     int sidx, 
    //     int tidx, 
    //     int num,
    //     vector<PendInfo> &pend, 
    //     vector<int> &pack
    // );

};

}

#endif