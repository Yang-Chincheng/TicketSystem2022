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
    int len;
    if(!rnum.Get(usr, len)) len = 0;
    record.Set(
        getTraxID(usr, len++), 
        TraxInfo(sta, id, st, tm, lv, ar, num, day, price, sidx, tidx)
    );
    rnum.Set(usr, len);
    return len;
}

int TraxManager::append_pending(const TrainID &id, int day, const Username &usr, int idx, int sidx, int tidx, int num)
{
// std::cerr << "APPEND_PENDING " << id << " " << day << std::endl;
    int len;
    if(!pnum.Get(make_pair(id, day), len)) len = 0;
    pending.Set(
        getPendID(id, day, len++),
        PendInfo(usr, idx, sidx, tidx, num, 0)
    );
    pnum.Set(make_pair(id, day), len);
    return len;
}

int TraxManager::pop_record(const Username &usr) {
    int num;
    assert(rnum.Get(usr, num));
    assert(num > 0);
    record.Delete(getTraxID(usr, --num));
    rnum.Set(usr, num);
    return num;
}

int TraxManager::pop_pending(const TrainID &id, int day) {
    int num;
    assert(pnum.Get(make_pair(id, day), num));
    assert(num > 0);
    pending.Delete(getPendID(id, day, --num));
    pnum.Set(make_pair(id, day), num);
    return num;
}

// idx: 0-base !!
int TraxManager::query_record(const Username &usr, int idx, bool rev, TraxInfo &pack) {
    int num;
    if(!rnum.Get(usr, num) || idx < 1 || idx > num) {
        throw transaction_error("record not found");
    }
    record.Get(getTraxID(usr, rev? num - idx: idx - 1), pack);
    return 0;
}

int TraxManager::query_record(const Username &usr, vector<TraxPack> &pack) {
    pack.clear(); int num; 
    if(!rnum.Get(usr, num) || !num) return 0;
    auto iter = record.Search(getTraxID(usr, 0)).first;
    for(int i = 0; i < num; ++i) pack.push_back(*iter), ++iter;
    return 0;
}

int TraxManager::query_pending(const TrainID &id, int day, vector<PendInfo> &pack) {
    pack.clear(); int num; 
// if(id == "LeavesofGrass" && day == 6) std::cerr << "Here!" << std::endl;
    if(!pnum.Get(make_pair(id, day), num)) return 0;
// std::cerr << "PENDING " << num << std::endl;
    if(!num) return 0;
    auto iter = pending.Search(getPendID(id, day, 0)).first;
    for(int i = 0; i < num; ++i) pack.push_back(*iter), ++iter;
    return 0;
}

int TraxManager::update_status(const Username &usr, int idx, bool rev, const Status &new_sta) {
    int num;
    if(!rnum.Get(usr, num) || idx > num) {
        throw transaction_error("record not found");
    }
    TraxInfo rec;
    assert(record.Get(getTraxID(usr, rev? num - idx: idx - 1), rec));
    rec.status = new_sta;
    record.Set(getTraxID(usr, rev? num - idx: idx - 1), rec);
    return 0;
}

int TraxManager::flip_masking(const TrainID &id, int day, vector<int> idx) {
    PendInfo pend;
    for(int i: idx) {
        assert(pending.Get(getPendID(id, day, i), pend));
        pend.mask ^= 1;
        pending.Set(getPendID(id, day, i), pend);
    }
    return 0;
}

int TraxManager::clear() {
    rnum.Clear();
    pnum.Clear();
    record.Clear();
    pending.Clear();
    return 0;
}

}
