#ifndef _TICKET_SYSTEM_TRAIN_H_
#define _TICKET_SYSTEM_TRAIN_H_

#include "../lib/utility.h"
#include "../lib/cached_bptree.h"
#include "../lib/hashmap.h"
#include "../lib/vector.h"
#include <iostream>

#define TRAIN_ROLLBACK 1

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

const int block_bit = 4;
const int block_siz = 2 << block_bit;
const int block_num = ((max_stanum - 1) >> block_bit) + 1;

#define __id(_x) ((_x) >> block_bit)
#define __lb(_x) ((_x) << block_bit)
#define __rb(_x) (((_x + 1) << block_bit) - 1)

struct SeatInfo {
    int val[block_num];
    int tag[block_num];
    int seat[block_num << block_bit];

    SeatInfo() = default;
    SeatInfo(const SeatInfo &o) = default;
    SeatInfo(int _num, int _seat);

    int query_seat(int s, int t);
    int modify_seat(int s, int t, int del);
};

struct PassInfo {
    Date st_date;
    int dura;
    Time arri_time;
    int stop_time;
    int pre_price;
    TrainID id;
    int idx;
    int hint;

    PassInfo() = default;
    PassInfo(const PassInfo &o) = default;
    PassInfo(
        const Date &_st,
        int _dura,
        const Time &_arri,
        const Time &_leav,
        int _pre,
        const TrainID &_id,
        int _idx,
        int _hint
    ):
    st_date(_st), dura(_dura), arri_time(_arri), stop_time(_leav - _arri), 
    pre_price(_pre), id(_id), idx(_idx), hint(_hint) {}

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
    // train information
    cached_bptree<size_t, TrainInfo> train;
    // numbers of rest seats of a certain train in a certain day
    cached_bptree<pair<size_t, int>, SeatInfo, PairHasher<size_t, int>> seat;
    // information of trains passing certain stations
    cached_bptree<pair<size_t, size_t>, PassInfo, PairHasher<size_t, size_t>> pass;

    int clear_train();

    int rollback_train(int opt_idx);

public:
    TrainManager(): train("train", 2048), seat("seat", 2048), pass("pass", 2048) {}
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


};

}

#endif