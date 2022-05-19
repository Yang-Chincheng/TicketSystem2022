#include "transaction.h"
#include <iostream>

#define SPLIT_INTO_VOLUMES 0

namespace ticket {

std::ostream& operator << (std::ostream &os, const TraxPack &pack) {
    if(pack.status == SUCCESS) os << "[success] ";
    if(pack.status == PENDING) os << "[pending] ";
    if(pack.status == REFUNDED) os << "[refunded] ";
    os << pack.id << " ";
    os << pack.start << " " << pack.leaving.date << " " << pack.leaving;
    os << " -> ";
    os << pack.termi << " " << pack.arriving.date << " " << pack.arriving;
    os << " " << pack.price << " " << pack.number; 
    return os;
}

int TraxManager::append_record(const Username &usr, const Status &sta, const TrainID &id, const Station &st, const Station &tm, const Time &lv, const Time &ar, int num, int price, int day, int sidx, int tidx) 
{
    TraxList list;
    if(!record.get(usr, list)) list.len = 0;
    assert(list.len < max_rec);
    list.seq[list.len++] = TraxInfo(sta, id, st, tm, lv, ar, num, day, price, sidx, tidx);
    record.insert(usr, list);
    return list.len;
}

int TraxManager::append_pending(const TrainID &id, int day, const Username &usr, int idx, int sidx, int tidx, int num)
{
    PendList list;
    if(!pending.get(make_pair(id, day), list)) list.len = 0;
    assert(list.len < max_pnd);
    list.seq[list.len++] = PendInfo(usr, idx, sidx, tidx, num, 0);
    pending.insert(make_pair(id, day), list);
    return list.len;
}

int TraxManager::pop_record(const Username &usr) {
    TraxList list;
    assert(record.get(usr, list));
    assert(list.len > 0);
    int ret = --list.len;
    record.insert(usr, list);
    return ret;
}

int TraxManager::pop_pending(const TrainID &id, int day) {
    PendList list;
    assert(pending.get(make_pair(id, day), list));
    assert(list.len > 0);
    int ret = --list.len;
    pending.insert(make_pair(id, day), list);
    return ret;
}

int TraxManager::query_record(const Username &usr, int idx, TraxPack &pack) {
    TraxList list;
    assert(record.get(usr, list));
    assert(idx >= 0 && idx < list.len);
    pack = TraxPack(list.seq[idx]);
    return 0;
}

int TraxManager::query_record(const Username &usr, vector<TraxPack> &pack) {
    TraxList list;
    assert(record.get(usr, list));
    pack.clear();
    for(int i = 0; i < list.len; ++i) pack.push_back(TraxPack(list.seq[i]));
    return 0;
}

int TraxManager::query_pending(const TrainID &id, int day, vector<PendPack> &pack) {
    PendList list;
    assert(pending.get(make_pair(id, day), list));
    pack.clear();
    for(int i = 0; i < list.len; ++i) {
        if(!list.seq[i].mask) pack.push_back(PendPack(list.seq[i]));
    }
    return 0;
}

int TraxManager::update_status(const Username &usr, int idx, const Status &new_sta) {
    TraxList list;
    assert(record.get(usr, list));
    assert(idx >= 0 && idx < list.len);
    list.seq[idx].status = new_sta;
    return 0;
}

int TraxManager::flip_masking(const TrainID &id, int day, vector<int> idx) {
    PendList list;
    assert(pending.get(make_pair(id, day), list));
    for(auto i: idx) {
        assert(i >= 0 && i < list.len);
        list.seq[i].mask ^= 1;
    }
    pending.insert(make_pair(id, day), list);
    return 0;
}

}
