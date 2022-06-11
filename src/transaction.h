#ifndef _TICKET_SYSTEM_TRAX_H_
#define _TICKET_SYSTEM_TRAX_H_

#include "../lib/utility.h"
#include "../lib/vector.h"
#include "../lib/cached_bptree.h"
#include <iostream>

#ifdef TICKSYS_ROLLBACK
    #define TRAX_ROLLBACK 1
#else 
    #define TRAX_ROLLBACK 1
#endif

namespace ticket {

struct PendInfo {
    int opt_time;
    size_t user; int idx;
    int sidx, tidx, num;

    PendInfo() = default;
    PendInfo(const PendInfo &o) = default;
    PendInfo(
        int _opt_idx,
        size_t &_user,
        int _idx,
        int _sidx,
        int _tidx,
        int _num
    ): 
    opt_time(_opt_idx), user(_user), idx(_idx), sidx(_sidx), tidx(_tidx), num(_num) {}

};

enum Status {SUCCESS, PENDING, REFUNDED};
struct TraxInfo {
    int opt_time;
    Status status;
    TrainID id;
    Station start;
    Station termi;
    Time leaving;
    Time arriving;
    int number, price;
    int day, sidx, tidx;

    TraxInfo() = default;
    TraxInfo(const TraxInfo &o) = default;
    TraxInfo(
        int _opt_idx,
        const Status &_sta,
        const TrainID &_id,
        const Station &_s,
        const Station &_t,
        const Time &_lv,
        const Time &_ar,
        int _num, 
        int _pri,
        int _day, 
        int _sidx, 
        int _tidx
    ):
    opt_time(_opt_idx), status(_sta), id(_id), 
    start(_s), termi(_t), leaving(_lv), arriving(_ar), 
    number(_num), day(_day), price(_pri), sidx(_sidx), tidx(_tidx) {}

};

using TraxID = pair<size_t, int>;
using PendID = pair<pair<size_t, int>, int>;

using TraxHasher = PairHasher<size_t, int>;
using PendHasher = PairHasher<pair<size_t, int>, int, PairHasher<size_t, int>>;

inline TraxID getTraxID(size_t usr, int idx) {
    return make_pair(usr, idx);
}
inline PendID getPendID(size_t id, int day, int idx) {
    return make_pair(make_pair(id, day), idx);
}

class TraxManager {
protected:
    cached_bptree<size_t, int> rnum;
    cached_bptree<TraxID, TraxInfo, TraxHasher> record;
    cached_bptree<PendID, PendInfo, PendHasher> pending;
    // BPTree<size_t, int> rnum;
    // BPTree<TraxID, TraxInfo> record;
    // BPTree<PendID, PendInfo> pending;

    int clear_trax();

    int rollback_trax(int opt_idx);

public:
    TraxManager(): rnum("rnum", 128), record("record"), pending("pending") {}
    TraxManager(const TraxManager &o) = delete;
    
};

}

#endif