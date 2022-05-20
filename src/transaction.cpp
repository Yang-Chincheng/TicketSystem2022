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

TraxManager::TraxID TraxManager::getTraxID(const Username &usr, int idx) {
    return make_pair(usr, idx);
}
TraxManager::PendID TraxManager::getPendID(const TrainID &tr, int day, int idx) {
    return make_pair(make_pair(tr, day), idx);
}

int TraxManager::append_record(const Username &usr, const Status &sta, const TrainID &id, const Station &st, const Station &tm, const Time &lv, const Time &ar, int num, int price, int day, int sidx, int tidx) 
{
    int num;
    if(!recnum.get(usr, num)) num = 0;
    recnum.insert(usr, ++num);
    TraxInfo rec(sta, id, st, tm, lv, ar, num, day, price, sidx, tidx);
    record.insert(getTraxID(usr, num), rec);
    return num;
}

int TraxManager::append_pending(const TrainID &id, int day, const Username &usr, int idx, int sidx, int tidx, int num)
{
    int num;
    if(!pendnum.get(make_pair(id, day), num)) num = 0;
    pendnum.insert(make_pair(id, day), ++num);
    PendInfo pend(usr, idx, sidx, tidx, num, 0);
    pending.insert(getPendID(id, day, num), pend);
    return num;
}

int TraxManager::pop_record(const Username &usr) {
    int num;
    assert(recnum.get(usr, num));
    assert(num > 0);
    recnum.insert(usr, --num);
    return num;
}

int TraxManager::pop_pending(const TrainID &id, int day) {
    int num;
    assert(pendnum.get(make_pair(id, day), num));
    assert(num > 0);
    pendnum.insert(make_pair(id, day), --num);
    return num;
}

// idx: 0-base !!
int TraxManager::query_record(const Username &usr, int idx, TraxPack &pack) {
    int num;
    if(!recnum.get(usr, num) || idx >= num) {
        throw transaction_error("record not found");
    }
    TraxInfo rec;
    record.get(getTraxID(usr, idx), rec);
    pack = TraxPack(rec);
    return 0;
}

int TraxManager::query_record(const Username &usr, vector<TraxPack> &pack) {
    int num;
    if(!recnum.get(usr, num)) {
        throw transaction_error("record not found");
    }
    TraxInfo rec;
    pack.clear();
    for(int i = 0; i < num; ++i) {
        assert(record.get(getTraxID(usr, i), rec));
        pack.push_back(TraxPack(rec));
    }
    return 0;
}

int TraxManager::query_pending(const TrainID &id, int day, vector<PendPack> &pack) {
    int num;
    assert(pendnum.get(make_pair(id, day), num));
    PendInfo pend;
    pack.clear();
    for(int i = 0; i < num; ++i) {
        assert(pending.get(getPendID(id, day, i), pend));
        if(!pend.mask) pack.push_back(PendPack(pend));
    }
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
    for(auto i: idx) {
        assert(pending.get(getPendID(id, day, i), pend));
        pend.mask ^= 1;
        pending.insert(getPendID(id, day, i), pend);
    }
    return 0;
}

}
