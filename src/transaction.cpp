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
    int num;
    if(!rnum.get(usr, num)) num = 0;
    record.insert(
        getTraxID(usr, num++), 
        TraxInfo(sta, id, st, tm, lv, ar, num, day, price, sidx, tidx)
    );
    rnum.insert(usr, num);
    return num;
}

int TraxManager::append_pending(const TrainID &id, int day, const Username &usr, int idx, int sidx, int tidx, int num)
{
    int num;
    if(!pnum.get(make_pair(id, day), num)) num = 0;
    pending.insert(
        getPendID(id, day, num++),
        PendInfo(usr, idx, sidx, tidx, num, 0)
    );
    pnum.insert(make_pair(id, day), num);
    return num;
}

int TraxManager::pop_record(const Username &usr) {
    int num;
    assert(rnum.get(usr, num));
    assert(num > 0);
    record.erase(getTraxID(usr, --num));
    rnum.insert(usr, num);
    return num;
}

int TraxManager::pop_pending(const TrainID &id, int day) {
    int num;
    assert(pnum.get(make_pair(id, day), num));
    assert(num > 0);
    pending.erase(getPendID(id, day, --num));
    pnum.insert(make_pair(id, day), num);
    return num;
}

// idx: 0-base !!
int TraxManager::query_record(const Username &usr, int idx, TraxPack &pack) {
    int num;
    if(!rnum.get(usr, num) || idx >= num) {
        throw transaction_error("record not found");
    }
    TraxInfo rec;
    record.get(getTraxID(usr, idx), rec);
    pack = TraxPack(rec);
    return 0;
}

int TraxManager::query_record(const Username &usr, vector<TraxPack> &pack) {
    pack.clear(); int num; 
    if(!rnum.get(usr, num) || !num) return 0;
    bptree<TraxID, TraxInfo>::iterator iter = record.find(getTraxID(usr, 0));
    for(int i = 0; i < num; ++i) pack.push_back(*iter), ++iter;
    return 0;
}

int TraxManager::query_pending(const TrainID &id, int day, vector<PendPack> &pack) {
    pack.clear(); int num; 
    if(!pnum.get(make_pair(id, day), num) || !num) return 0;
    bptree<PendID, PendInfo>::iterator iter = pending.find(getPendID(id, day, 0));
    for(int i = 0; i < num; ++i) pack.push_back(*iter), ++iter;
    return 0;
}

int TraxManager::update_status(const Username &usr, int idx, const Status &new_sta) {
    TraxInfo rec;
    assert(record.get(getTraxID(usr, idx), rec));
    rec.status = new_sta;
    record.insert(getTraxID(usr, idx), rec);
    return 0;
}

int TraxManager::flip_masking(const TrainID &id, int day, vector<int> idx) {
    PendInfo pend;
    for(int i: idx) {
        assert(pending.get(getPendID(id, day, i), pend));
        pend.mask ^= 1;
        pending.insert(getPendID(id, day, i), pend);
    }
    return 0;
}

int TraxManager::clear() {
    rnum.clear();
    pnum.clear();
    record.clear();
    pending.clear();
    return 0;
}

}
