#ifndef _TICKET_SYSTEM_TRAIN_H_
#define _TICKET_SYSTEM_TRAIN_H_

#include "../lib/utility.h"
#include "../lib/BPlusTree.h"
#include "../lib/hashmap.h"
#include "../lib/vector.h"
#include <iostream>

namespace ticket {

// Manager
class TrainManager;

// Infomation Preserver
struct LineInfo;
struct TrainInfo;
struct MetaData;
struct PassTrain;

// [New] Infomation Transmitter
struct LinePack;
struct TravelPack;
using TransPack = pair<TravelPack, TravelPack>;
struct TicketPack;

const int max_stanum = 102;
const int max_date = 100;
const int max_pnum = 10000;

struct LineInfo {
    int sta_num;
    Station sta[max_stanum];
    int price[max_stanum];
    int arri_time[max_stanum];
    int leav_time[max_stanum];

    LineInfo() = default;
    LineInfo(
        int _stanum,  
        Station *_sta,
        int *_price,
        const Time &_st_time,
        int *_tra_time,
        int *_stp_time
    );
    LineInfo(const LineInfo &o) = default;
    ~LineInfo() = default;

};

struct LinePack: public LineInfo, public InfoPack {
    TrainID id;
    Time st_time;
    char type;
    int seat[max_stanum];
    LinePack() = default;
    LinePack(
        const TrainID &_id,
        const LineInfo &info, 
        const Time &_st_time, 
        char _type, 
        int *_seat
    ): id(_id), LineInfo(info), st_time(_st_time) {
        type = _type;
        for(int i = 1; i < info.sta_num; ++i) seat[i] = _seat[i];
    }
    friend std::ostream& operator << (std::ostream &os, const LinePack &info);

};

struct TrainInfo {
    bool released;
    char type;
    int sta_num; // 1-base
    int day_num; // 0-base
    Time st_time;
    Date st_date, ed_date;
    LineInfo line;
    int seat[max_date][max_stanum];

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
    int query_seat(int d, int s, int t);
    int modify_seat(int d, int s, int t, int del);
    int search_station(const Station &sta, int st, int ed);

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
        const Station &_strt, const Station &_term, 
        const Time &_leav, const Time &_arri,
        int _price, int _seat
    ): id(_id), strt(_strt), term(_term), leav(_leav), arri(_arri) {
        price = _price, seat = _seat;
    }

    friend std::ostream& operator << (std::ostream &os, const TravelPack &pack);

};

struct TicketPack: public InfoPack {
    int day, sidx, tidx;
    int price, seat;
    Time leave, arrive;
};

#ifndef _RECORD_PENDING_
#define _RECORD_PENDING_
struct PendInfo {
    Username user; int idx;
    int sidx, tidx, num;
    bool mask;

    PendInfo() = default;
    PendInfo(const PendInfo &o) = default;
    PendInfo(
        const Username &_user,
        int _idx,
        int _sidx,
        int _tidx,
        int _num,
        bool _mask
    ): 
    user(_user), idx(_idx), sidx(_sidx), tidx(_tidx), num(_num), mask(_mask) {}

};
#endif

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
        int t2 = rhs.second.arri - lhs.first.leav;
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
        int t2 = rhs.second.arri - lhs.first.leav;
        if(t1 != t2) return t1 < t2;
        if(lhs.first.id != rhs.first.id) return lhs.first.id < rhs.first.id;
        return lhs.second.id < rhs.second.id;
    }    
};

struct PassTrain {
    struct MetaData {
        Date st_date, ed_date;
        TrainID id;
        int idx;
        BPTree<TrainID, TrainInfo>::Iterator iter;
    };
    int pnum;
    MetaData ptrain[max_pnum]; // 0-base
};

class TrainManager {
private:
    BPTree<TrainID, TrainInfo> train;
    BPTree<Station, PassTrain> station;

public:
    TrainManager(): train("train"), station("station") {}
    TrainManager(const TrainManager &o) = delete;
    ~TrainManager() = default;

    int add_train(
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

    int delete_train(const TrainID& id);

    int release_train(const TrainID &id);
    
    int query_train(
        const TrainID& id, 
        const Date &date,
        LinePack &pack
    );

    int query_ticket(
        const Station &strt,
        const Station &term,
        const Date &date,
        bool cmp_type,
        vector<TravelPack> &pack
    );

    int query_transfer(
        const Station &strt,
        const Station &term,
        const Date &date,
        bool cmp_type,
        TransPack &pack
    );

    int check_ticket(
        const TrainID &id,
        const Date &date, 
        const Station &strt,
        const Station &term,
        int num,
        TicketPack &pack
    );

    int check_refund(
        const TrainID &id,
        int day, 
        int sidx, 
        int tidx, 
        int num,
        vector<PendInfo> &pend, 
        vector<int> &pack
    );

    int clear();

};

}

#endif