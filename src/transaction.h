#ifndef _TICKET_SYSTEM_TRAX_H_
#define _TICKET_SYSTEM_TRAX_H_

#include "../lib/utility.h"
#include "../lib/vector.h"
#include "../test/bptree.h"
#include <iostream>

#define SPLIT_INTO_VOLUMES 0

namespace ticket {

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
struct PendPack: public InfoPack {
    Username user; int idx;
    int sidx, tidx, num;

    PendPack() = default;
    PendPack(const PendPack &o) = default;
    PendPack(const PendInfo &info):
    user(info.user), idx(info.idx), 
    sidx(info.sidx), tidx(info.tidx), num(info.num) {}

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
    int number;
    int day, price, sidx, tidx;

    TraxInfo() = default;
    TraxInfo(const TraxInfo &o) = default;
    TraxInfo(
        const Status &_sta,
        const TrainID &_id,
        const Station &_s,
        const Station &_t,
        const Time &_lv,
        const Time &_ar,
        int _num, 
        int _day, 
        int _pri,
        int _sidx, 
        int _tidx
    ):
    status(_sta), id(_id), start(_s), termi(_t), leaving(_lv), arriving(_ar), 
    number(_num), day(_day), price(_pri), sidx(_sidx), tidx(_tidx) {}

};

struct TraxPack: public TraxInfo, public InfoPack {
    Status status;
    TrainID id;
    Station start;
    Station termi;
    Time leaving;
    Time arriving;
    int price;
    int number;
    
    TraxPack() = default;
    TraxPack(const TraxPack &o) = default;
    TraxPack(const TraxInfo &info):
    status(info.status), id(info.id), start(info.start), termi(info.termi),
    leaving(info.leaving), arriving(info.arriving), number(info.number), price(info.price) {}

    friend std::ostream& operator << (std::ostream &os, const TraxPack &pack);

};

const int max_rec = 1e5;
const int max_pnd = 1e5;

struct PendList {
    int len;
    PendInfo seq[max_pnd];
};
struct TraxList {
    int len;
    TraxInfo seq[max_rec];
};

class TraxManager {
private:
    bptree<Username, TraxList> record;
    bptree<pair<TrainID, int>, PendList> pending;

public:
    TraxManager(): record("record"), pending("pending") {}
    TraxManager(const TraxManager &o) = delete;

    int append_record(
        const Username &usr,
        const Status &sta,
        const TrainID &id,
        const Station &st,
        const Station &tm,
        const Time &lv,
        const Time &ar,
        int num,
        int price,
        int day,
        int sidx, 
        int tidx
    );

    int append_pending(
        const TrainID &train,
        int day, 
        const Username &user,
        int idx,
        int sidx, 
        int tidx,
        int num
    );

    int pop_record(
        const Username &usr
    );

    int pop_pending(
        const TrainID &train, 
        int day
    );

    int query_record(
        const Username &usr,
        int idx,
        TraxPack &pack
    );

    int query_record(
        const Username &usr,
        vector<TraxPack> &pack
    );

    int query_pending(
        const TrainID &id,
        int day, int idx,
        PendPack &pack
    );

    int query_pending(
        const TrainID &id,
        int day,
        vector<PendPack> &pack
    );

    int update_status(
        const Username &usr,
        int idx, 
        const Status &new_sta 
    );

    int flip_masking(
        const TrainID &id,
        int day,
        vector<int> idx
    );
    
};

}

#endif