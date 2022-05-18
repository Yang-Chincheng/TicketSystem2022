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

#if SPLIT_INTO_VOLUMES
int TraxManager::add_record(const Username &_usr, const Status &_sta, const TrainID &_id, const Station &_st, const Station &_tm, const Time &_lv, const Time &_ar, int _pri, int _num) 
{
    static int cnt = 0;
    iterator _beg = rec.lower_bound(pui(_usr, 0));
    iterator _end = rec.upper_bound(pui(_usr, 1e8));
    if(_beg == _end) {
        rec.insert(
            pui(_usr, cnt),
            (TraxList) {
                1, cnt, {TraxInfo(_sta, _id, _st, _tm, _lv, _ar, _pri, _num)}
            }
        );
        cnt++;
        return 1;
    }
    iterator _itr = --_end;
    TraxList list = *_itr;
    if(list.rnum == max_rec) {
        rec.insert(
            pui(_usr, cnt),
            (TraxList) {
                1, cnt, {TraxInfo(_sta, _id, _st, _tm, _lv, _ar, _pri, _num)}
            }
        );
        cnt++;
        return 1;
    }
    list.rec[list.rnum++] = TraxInfo(_sta, _id, _st, _tm, _lv, _ar, _pri, _num);
    rec.insert(pui(_usr, list.id), list);
    return 1;
}

#else
int TraxManager::add_record(const Username &_usr, const Status &_sta, const TrainID &_id, const Station &_st, const Station &_tm, const Time &_lv, const Time &_ar, int _pri, int _num, int &cnt) 
{
    TraxList list;
    if(!record.get(_usr, list)) list.rnum = 0;
    list.rec[list.rnum++] = TraxInfo(_sta, _id, _st, _tm, _lv, _ar, _pri, _num);
    record.insert(_usr, list), cnt = list.rnum;
    return 0;
}

int TraxManager::change_status(const Username &_usr, const Status &new_sta, int _idx) {
    TraxList list;
    if(_idx <= 0 || !record.get(_usr, list) || list.rnum < _idx) {
        throw transaction_error("record not found");
    }
    Status &cur_sta = list.rec[_idx - 1].status;
    if(cur_sta == REFUNDED) {
        throw transaction_error("illegal status change");
    }
    if(cur_sta == SUCCESS && new_sta != REFUNDED) {
        throw transaction_error("illegal status change");
    }
    if(cur_sta == PENDING && new_sta != SUCCESS) {
        throw transaction_error("illegal status change");
    }
    cur_sta = new_sta;
    record.insert(_usr, list);
    return 0;
}

int TraxManager::delete_record(const Username &_usr) {
    TraxList list;
    if(!record.get(_usr, list) || !list.rnum) {
        throw transaction_error("empty transaction list");
    }
    list.rnum--;
    record.insert(_usr, list);
    return 0;
}

int TraxManager::query_order(const Username &_usr, vector<TraxPack> &pack) {
    TraxList list; 
    pack.clear();
    if(!record.get(_usr, list)) return 0;
    for(int i = 0; i < list.rnum; ++i) {
        pack.push_back(list.rec[i]);
    }
    return 0;
}

#endif

}
