
#include "train.h"
#include <iostream>

namespace ticket {

LineInfo::LineInfo(int _stanum, Station *_sta, int *_price, const Time &_st_time, int *_tra_time, int *_stp_time) 
{
    sta_num = _stanum;
    for(int i = 1; i <= sta_num; ++i) sta[i] = _sta[i];
    price[0] = price[1] = 0;
    for(int i = 2; i <= sta_num; ++i) price[i] = price[i - 1] + _price[i];
    _stp_time[1] = 0;
    _tra_time[_stanum] = 0;
    int time = 0;
    for(int i = 1; i <= sta_num; ++i) {
        arri_time[i] = time, time += _stp_time[i];
        leav_time[i] = time, time += _tra_time[i];
    }
}

TrainInfo::TrainInfo(int _stanum, int _seatnum, Station *_sta, int *_price, const Time &_st_time, int *_tra_time, int *_stp_time, const Date &_st_date, const Date &_ed_date, char _type)
: line(_stanum, _sta, _price, _st_time, _tra_time, _stp_time) 
{
    released = 0;
    type = _type;
    sta_num = _stanum;
    day_num = (_ed_date - _st_date) + 1;
    st_time = _st_time;
    st_date = _st_date, ed_date = _ed_date;
    for(int i = 0; i < day_num; ++i) {
        for(int j = 1; j < sta_num; ++j) seat[i][j] = _seatnum;
    }
}

Time TrainInfo::arrive_time(int day, int idx) {
    if(day < 0) day += day_num;
    assert(idx >= 1);
    // if(idx < 0) idx += sta_num;
    Time time = st_time;
    time.date = st_date + day;
    time += line.arri_time[idx];
    return time;
}

Time TrainInfo::leave_time(int day, int idx) {
    assert(day_num > 0);
    if(day < 0) day += day_num;
    assert(day >= 0 && day < day_num);
    assert(idx >= 1);
    // if(idx < 0) idx += sta_num;
    Time time = st_time;
    time.date = st_date + day;
    time += line.leav_time[idx];
    return time;
}

int TrainInfo::total_price(int s, int t) {
    return line.price[t] - line.price[s];
}

int TrainInfo::query_seat(int d, int s, int t) {
    if(d < 0) d += day_num;
    int res = 0x7fffffff;
    for(int i = s; i < t; ++i) res = std::min(res, seat[d][i]);
    return res;
}

int TrainInfo::modify_seat(int d, int s, int t, int del) {
    if(d < 0) d += day_num;
    for(int i = s; i < t; ++i) seat[d][i] += del;
    return 0;
}

int TrainInfo::search_station(const Station &sta, int st = 1, int ed = 0) {
    if(!ed) ed = sta_num;
    for(int i = st; i <= ed; ++i) if(line.sta[i] == sta) return i;
    return 0;
}

std::ostream& operator << (std::ostream &os, const LinePack &pack) {
    os << pack.id << " " << pack.type << std::endl;
    Time cur;
    for(int i = 1; i <= pack.sta_num; ++i) {
        // station name
        os << pack.sta[i] << " ";
        // arriving time
        cur = pack.st_time + pack.arri_time[i];
        if(i == 1) os << "xx-xx xx:xx";
        else os << cur.date << " " << cur;
        os << " -> ";
        // leaving time
        cur = pack.st_time + pack.leav_time[i];
        if(i == pack.sta_num) os << "xx-xx xx:xx ";
        else os << cur.date << " " << cur << " ";
        // accumulated price
        os << pack.price[i] << " ";
        // number of rest seat
        if(i == pack.sta_num) os << "x" << std::endl;
        else os << pack.seat[i] << std::endl;
    }
    return os;
}

std::ostream& operator << (std::ostream &os, const TravelPack &pack) {
    os << pack.id << " " << pack.strt << " " << pack.leav.date << " " << pack.leav << " -> ";
    os << pack.term << " " << pack.arri.date << " " << pack.arri << " ";
    os << pack.price << " " << pack.seat;
    return os;
}

int TrainManager::add_train(const TrainID &id, int _stanum, int _seatnum, Station *_sta, int *_price, const Time &_st_time, int *_tra_time, int *_stp_time, const Date &_st_date, const Date &_ed_date, char _type) 
{
    if(train.Search(id).second) {
        throw train_error("train already exists, addition failed");
    }
    TrainInfo new_train(
        _stanum, _seatnum, _sta, _price, _st_time, _tra_time, _stp_time, _st_date, _ed_date, _type
    );
// if(id == "LeavesofGrass") {
//     std::cout << "<TRAIN>\n";
//     for(int i = 1; i <= new_train.sta_num; ++i) {
//         std::cout << new_train.line.sta[i] << " ";
//         std::cout << new_train.arrive_time(0, i).date << " " << new_train.arrive_time(0, i) << " ";
//         std::cout << new_train.leave_time(0, i).date << " " << new_train.leave_time(0, i) << std::endl;
//     }
// }
    train.Set(id, new_train);
    return 0;
}

int TrainManager::delete_train(const TrainID &id) {
    TrainInfo tr;
    if(!train.Get(id, tr)) {
        throw train_error("train not found");
    }
    if(tr.released) {
        throw train_error("the train has released, deletion failed");
    }
    train.Delete(id);
    return 0;
}

int TrainManager::release_train(const TrainID &id) {
    TrainInfo tr;
    if(!train.Get(id, tr)) {
        throw train_error("train not found");
    }
    if(tr.released) {
        throw train_error("the train has released");
    }
    // set release tag as 1
    tr.released = 1;
    train.Set(id, tr);

    // update by-pass train list
    Station sta;
    PassTrain ptra;
    for(int i = 1; i <= tr.sta_num; ++i) {
        sta = tr.line.sta[i];
        auto iter = train.Search(id).first;
        if(station.Get(sta, ptra)) {
            assert(ptra.pnum < max_pnum);
            ptra.ptrain[ptra.pnum++] = (PassTrain::MetaData) {
                tr.leave_time(0, i).date, tr.leave_time(-1, i).date, id, i, iter
            };
        }
        else {
            ptra.pnum = 1;
            ptra.ptrain[0] = (PassTrain::MetaData) {
                tr.leave_time(0, i).date, tr.leave_time(-1, i).date, id, i, iter
            };
        }
        station.Set(sta, ptra);
    }
    return 0;
}

int TrainManager::query_train(const TrainID &id, const Date &date, LinePack &pack) {
    TrainInfo tr;
    if(!train.Get(id, tr)) {
        throw train_error("train not found");
    }
    if(date < tr.st_date || date > tr.ed_date) {
        throw train_error("no schedule in this day");
    }
    Time time(tr.st_time);
    time.date = date;
    pack = LinePack(id, tr.line, time, tr.type, tr.seat[date - tr.st_date]);
    return 0;
}

int TrainManager::query_ticket(const Station &strt, const Station &term, const Date &date, bool cmp_type, vector<TravelPack> &pack) {
    pack.clear();
    PassTrain ps;
    TrainInfo tr;
    // enumerate each train passing the start station
    if(!station.Get(strt, ps)) return 0;
    for(int i = 0; i < ps.pnum; ++i) {
        // rough check using abstract
        PassTrain::MetaData *ptr = ps.ptrain + i;
        if(date >= ptr->st_date && date <= ptr->ed_date) {
            // get detail train info
            tr = *ptr->iter;
            // detail check and get ans
            int sidx = ptr->idx;
            int tidx = tr.search_station(term, sidx);
            if(tidx) {
                int day = date - ptr->st_date;
                int res_seat = tr.query_seat(day, sidx, tidx);
                if(res_seat == 0) continue;
// if(ptr->id == "LeavesofGrass" && day == 22) {
// std::cerr << "QUERY_TICKET " << ptr->id << " " << day << " " << sidx << " " << tidx << std::endl;
// std::cout << "<TEST>\n";
// for(int i = 1; i < tr.sta_num; ++i) std::cout << tr.seat[day][i] << " "; 
// std::cout << std::endl;
// }
                pack.push_back(TravelPack(
                    ptr->id, strt, term,
                    tr.leave_time(day, sidx), tr.arrive_time(day, tidx),
                    tr.total_price(sidx, tidx),
                    res_seat
                ));
            }
        }
    }
    // sort the answer
    if(cmp_type) sort<TravelPack, ByCost>(pack, 0, pack.size());
    else sort<TravelPack, ByTime>(pack, 0, pack.size());
    return 0;
}

int TrainManager::query_transfer(const Station &strt, const Station &term, const Date &date, bool cmp_type, TransPack &pack) {
    bool tag = 0;
    PassTrain ps_s, ps_t;
    TrainInfo tr_s, tr_t;
    hashmap<Station, int, StrHasher> mp;
    
    if(!station.Get(strt, ps_s) || !station.Get(term, ps_t)) return -1;
    // enumerate all the train passing the start station
    for(int i = 0; i < ps_s.pnum; ++i) {
        PassTrain::MetaData *ptr_s = ps_s.ptrain + i;
        if(date >= ptr_s->st_date && date <= ptr_s->ed_date) {
            tr_s = *ptr_s->iter;
            
            int day_s = date - ptr_s->st_date;
            // enumerate all the train passin the terminal station
            for(int j = 0; j < ps_t.pnum; ++j) {
                PassTrain::MetaData *ptr_t = ps_t.ptrain + j;
                // trains shound be different 
                if(ptr_s->id == ptr_t->id) continue;
                
                tr_t = *ptr_t->iter;
                mp.clear();
                for(int k = ptr_s->idx + 1; k <= tr_s.sta_num; ++k) mp[tr_s.line.sta[k]] = k;
                for(int k = ptr_t->idx - 1; k >= 1; --k) {
                    // the exchange station
                    Station ex_sta = tr_t.line.sta[k];
                    int ex_idx = mp[ex_sta];
                    if(!ex_idx) continue;

                    // check route [tr1: sidx -> ex_idx] -> [tr2: k -> tidx]
                    Time arri_ex = tr_s.arrive_time(day_s, ex_idx);
                    if(arri_ex <= tr_t.leave_time(-1, k)) {
                        int day_t = std::max(0, arri_ex.date - tr_t.leave_time(0, k).date);
                        if(arri_ex > tr_t.leave_time(day_t, k)) day_t++;
                        int res_s = tr_s.query_seat(day_s, ptr_s->idx, ex_idx);
                        int res_t = tr_t.query_seat(day_t, k, ptr_t->idx);
                        if(!res_s || !res_t) continue;
                        TransPack cur(
                            TravelPack(
                                ptr_s->id,
                                strt, ex_sta,
                                tr_s.leave_time(day_s, ptr_s->idx),
                                arri_ex,
                                tr_s.total_price(ptr_s->idx, ex_idx),
                                res_s
                            ),
                            TravelPack(
                                ptr_t->id,
                                ex_sta, term,
                                tr_t.leave_time(day_t, k),
                                tr_t.arrive_time(day_t, ptr_t->idx),
                                tr_t.total_price(k, ptr_t->idx),
                                res_t
                            )
                        );
                        if(tag) {
                            if(cmp_type) cmin<TransPack, ByCost>(pack, cur);
                            else cmin<TransPack, ByTime>(pack, cur);
                        }
                        else pack = cur, tag = 1;
// if(strt == "广东省梅州市" && term == "河南省禹州市" && date == Date(7, 30) && cmp_type == 0) {
//     std::cerr << ">> " << cur.first << "\n" << cur.second << std::endl;
//     std::cerr << "note: " << pack.first << "\n" << pack.second << "\n" << std::endl;
// }
                    }
                }
            }
        }
    }
    return tag? 0: -1;
}

int TrainManager::check_ticket(const TrainID &id, const Date &date, const Station &st, const Station &tm, int num, TicketPack &pack) 
{
    TrainInfo tr;
    if(!train.Get(id, tr)) {
        throw transaction_error("ticket not found");
    }
    pack.sidx = tr.search_station(st);
    if(!pack.sidx) {
        throw transaction_error("ticket not found");
    }
    pack.tidx = tr.search_station(tm, pack.sidx);
    if(!pack.tidx) {
        throw transaction_error("ticket not found");
    }
    Date st_date = tr.leave_time( 0, pack.sidx).date;
    Date ed_date = tr.leave_time(-1, pack.sidx).date;
    if(date < st_date || date > ed_date) {
        throw transaction_error("ticket not found");
    }

    pack.day = date - st_date;
// assert(pack.day >= 0 && pack.day < tr.day_num);
    pack.leave  = tr.leave_time  (pack.day, pack.sidx);
    pack.arrive = tr.arrive_time (pack.day, pack.tidx);
    pack.price  = tr.total_price (pack.sidx, pack.tidx);
    pack.seat   = tr.query_seat  (pack.day, pack.sidx, pack.tidx);
// if(num == 23767) std::cout << "CHECKNUM " << num << " " << pack.seat << std::endl;
    if(num <= pack.seat) {
        tr.modify_seat(pack.day, pack.sidx, pack.tidx, -num);
        train.Set(id, tr);
    }
// if(id == "LeavesofGrass" && date - st_date == 22) {
// std::cout << "<TEST>\n";
// for(int i = 1; i < tr.sta_num; ++i) std::cout << tr.seat[date - st_date][i] << " "; 
// std::cout << std::endl;
// }
    return 0;
}

int TrainManager::check_refund(bool suc, const TrainID &id, int day, int sidx, int tidx, int num, vector<PendInfo> &pend, vector<int> &pack) 
{
// std::cerr << "CHECK_REFUND " << id << " " << day << " " << sidx << " " << tidx << " " << num  << std::endl;
    TrainInfo tr;
    assert(train.Get(id, tr));
    assert(day >= 0 && day < tr.day_num);
    assert(0 < sidx && sidx < tidx && tidx <= tr.sta_num);
    if(suc) tr.modify_seat(day, sidx, tidx, num);
    pack.clear();
    int i = 0;
    for(PendInfo &rec: pend) {
// std::cerr << "PENDING_RECORD " << rec.user << " " << rec.idx << " " << rec.sidx << " " << rec.tidx << " " << rec.num << " " << rec.mask << std::endl;
        if(!rec.mask && tr.query_seat(day, rec.sidx, rec.tidx) >= rec.num) {
            pack.push_back(i);
            tr.modify_seat(day, rec.sidx, rec.tidx, -rec.num);
        }
        i++;
    }
// if(id == "LeavesofGrass" && day == 22) {
// std::cout << "<TEST>\n";
// for(int i = 1; i < tr.sta_num; ++i) std::cout << tr.seat[day][i] << " "; 
// std::cout << std::endl;
// }
    train.Set(id, tr);
    return 0;
}

int TrainManager::clear() {
    train.Clear();
    station.Clear();
    return 0;
}

}