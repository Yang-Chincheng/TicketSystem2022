#ifndef _TICKET_SYSTEM_TRAX_H_
#define _TICKET_SYSTEM_TRAX_H_

#include "../lib/utility.h"
#include "../lib/vector.h"
#include "../test/bptree.h"
#include <iostream>

#define SPLIT_INTO_VOLUMES 0

namespace ticket {

#ifndef _TICKET_INFO_PACK_
#define _TICKET_INFO_PACK_
struct TicketPack: public InfoPack {
    int price, seat;
    TicketPack() = default;
    TicketPack(int _price, int _seat): price(_price), seat(_seat) {}
    TicketPack(const TicketPack &o) = default;
};
#endif

enum Status {SUCCESS, PENDING, REFUNDED};
struct TraxInfo {
    Status status;
    TrainID id;
    Station start;
    Station termi;
    Time leaving;
    Time arriving;
    int price, number;

    TraxInfo() = default;
    TraxInfo(const TraxInfo &o) = default;
    TraxInfo(
        const Status &_sta,
        const TrainID &_id,
        const Station &_s,
        const Station &_t,
        const Time &_lv,
        const Time &_ar,
        int _pri, 
        int _num
    ): 
    status(_sta), id(_id), start(_s), termi(_t), 
    leaving(_lv), arriving(_ar), price(_pri), number(_num) {}

};

struct TraxPack: public TraxInfo, public InfoPack {
    TraxPack() = default;
    TraxPack(const TraxPack &o) = default;
    TraxPack(
        const Status &_sta,
        const TrainID &_id,
        const Station &_s,
        const Station &_t,
        const Time &_lv,
        const Time &_ar,
        int _pri, 
        int _num
    ): TraxInfo(_sta, _id, _s, _t, _lv, _ar, _pri, _num) {}
    TraxPack(const TraxInfo &info): TraxInfo(info) {}

    friend std::ostream& operator << (std::ostream &os, const TraxPack &pack);

};

#if SPLIT_INTO_VOLUMES
const int max_rec = 1e4;

struct TraxList {
    int rnum, id;
    TraxInfo rec[max_rec];
};

class TraxManager {
private:
    using pui = pair<Username, int>;
    using iterator = typename bptree<pui, TraxList>::iterator;
    bptree<pui, TraxList> rec;

public:

    int add_record(
        const Username &_usr,
        const Status &_sta,
        const TrainID &_id,
        const Station &_st,
        const Station &_tm,
        const Time &_lv,
        const Time &_ar,
        int _pri,
        int _num
    );

    int change_status(
        const Username &_usr,
        const Status &new_sta,
        int _idx
    );

    int delete_record(const Username &_usr);

    int query_order(
        const Username &_usr, 
        vector<TraxInfo> &pack
    );

};
#else
const int max_rec = 1e6;

struct TraxList {
    int rnum;
    TraxInfo rec[max_rec];
};

class TraxManager {
private:
    bptree<Username, TraxList> record;

public:

    TraxManager(): record("user_record") {}
    TraxManager(const TraxManager &o) = delete;

    int add_record(
        const Username &_usr,
        const Status &_sta,
        const TrainID &_id,
        const Station &_st,
        const Station &_tm,
        const Time &_lv,
        const Time &_ar,
        int _pri,
        int _num,
        int &cnt
    );

    int change_status(
        const Username &_usr,
        const Status &new_sta,
        int _idx
    );

    int delete_record(const Username &_usr);

    int query_order(
        const Username &_usr, 
        vector<TraxPack> &pack
    );

};

#endif

}

#endif