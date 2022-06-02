
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
    tot_seat = _seatnum;
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

// void PassTrain::append(const Date &_st, const Date &_ed, const TrainID &_id, int _idx, BPTree<TrainID, TrainInfo>::Iterator &_iter) {
//     assert(pnum < max_pnum);
//     ptrain[pnum++] = MetaData(_st, _ed, _id, _idx, _iter);
// }

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
// if(id == "whatyouwanted") {
//     std::cerr << "<TRAIN>\n";
//     for(int i = 1; i <= new_train.sta_num; ++i) {
//         std::cerr << new_train.line.sta[i] << " ";
//         std::cerr << new_train.arrive_time(0, i).date << " " << new_train.arrive_time(0, i) << " ";
//         std::cerr << new_train.leave_time(0, i).date << " " << new_train.leave_time(0, i) << std::endl;
//     }
//     assert(0);
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
    int cur = 1;
    for(int i = 1; i <= tr.sta_num; ++i) {
        sta = tr.line.sta[i];
        int num;
        if(!pnum.Get(sta, num)) num = 0;
        pass.Set(
            make_pair(sta, num++),
            PassInfo(
                tr.leave_time(0, i).date, tr.leave_time(-1, i).date, id, i, cur
            )
        );
        pnum.Set(sta, num);
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
    PassInfo ps;
    TrainInfo tr;
    // enumerate each train passing the start station
    int num;
    if(!pnum.Get(strt, num) || !num) return 0;
    auto iter = pass.Search(make_pair(strt, 0)).first;
    for(int i = 0; i < num; ++i, ++iter) {
        ps = *iter;
        // rough check using abstract
        if(date >= ps.st_date && date <= ps.ed_date) {
            // get detail train info
// std::cerr << "when accessing train: " << ptr->id << std::endl;
// std::cerr << "[hint] " << (size_t) (&station) << " " << (size_t) (&train) << std::endl;
// assert(ptr->iter == train.Search(ptr->id).first);
            bool fb = train.Get(ps.id, tr);
            assert(fb);
            // detail check and get ans
            int sidx = ps.idx;
            int tidx = tr.search_station(term, sidx);
            if(tidx) {
                int day = date - ps.st_date;
                int res_seat = tr.query_seat(day, sidx, tidx);
                // if(res_seat == 0) continue;
// if(ptr->id == "LeavesofGrass" && day == 22) {
// std::cerr << "QUERY_TICKET " << ptr->id << " " << day << " " << sidx << " " << tidx << std::endl;
// std::cout << "<TEST>\n";
// for(int i = 1; i < tr.sta_num; ++i) std::cout << tr.seat[day][i] << " "; 
// std::cout << std::endl;
// }
                pack.push_back(TravelPack(
                    ps.id, strt, term,
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
    PassInfo ps_s, ps_t;
    TrainInfo tr_s, tr_t;
    hashmap<Station, int, StrHasher> mp;
    
    int num_s, num_t;
    if(!pnum.Get(strt, num_s) || !pnum.Get(term, num_t)) return -1;
    if(!num_s || !num_t) return -1;
// if(strt == "河北省安国市" && term == "河南省信阳市" && date == Date(7, 13)) { 
// std::cerr << "reached here " << num_s << " " << num_t << std::endl;
// }
    auto iter_s = pass.Search(make_pair(strt, 0)).first;
    // enumerate all the train passing the start station
    for(int i = 0; i < num_s; ++i, ++iter_s) {
        ps_s = *iter_s;
//  assert(iter_s == pass.Search(make_pair(strt, i)).first);
        if(date >= ps_s.st_date && date <= ps_s.ed_date) {
            bool fb = train.Get(ps_s.id, tr_s);
            assert(fb);
            int day_s = date - ps_s.st_date;
            // enumerate all the train passin the terminal station
            auto iter_t = pass.Search(make_pair(term, 0)).first;
            for(int j = 0; j < num_t; ++j, ++iter_t) {
                ps_t = *iter_t;
// assert(iter_t == pass.Search(make_pair(term, j)).first);
                // trains shound be different 
                if(ps_t.id == ps_s.id) continue;
                fb = train.Get(ps_t.id, tr_t);
                assert(fb);
// assert(tr_s.search_station(strt) == ps_s.idx);
// assert(tr_t.search_station(term) == ps_t.idx);
                mp.clear();
                for(int k = ps_s.idx + 1; k <= tr_s.sta_num; ++k) mp[tr_s.line.sta[k]] = k;
                for(int k = ps_t.idx - 1; k >= 1; --k) {
                    // the exchange station
                    Station ex_sta = tr_t.line.sta[k];
                    int ex_idx = mp[ex_sta];
                    if(!ex_idx) continue;

                    // check route [tr1: sidx -> ex_idx] -> [tr2: k -> tidx]
                    Time arri_ex = tr_s.arrive_time(day_s, ex_idx);
                    if(arri_ex <= tr_t.leave_time(-1, k)) {
                        int day_t = std::max(0, arri_ex.date - tr_t.leave_time(0, k).date);
                        if(arri_ex > tr_t.leave_time(day_t, k)) day_t++;
                        int res_s = tr_s.query_seat(day_s, ps_s.idx, ex_idx);
                        int res_t = tr_t.query_seat(day_t, k, ps_t.idx);
                        // if(!res_s || !res_t) continue;
                        TransPack cur(
                            TravelPack(
                                ps_s.id,
                                strt, ex_sta,
                                tr_s.leave_time(day_s, ps_s.idx),
                                arri_ex,
                                tr_s.total_price(ps_s.idx, ex_idx),
                                res_s
                            ),
                            TravelPack(
                                ps_t.id,
                                ex_sta, term,
                                tr_t.leave_time(day_t, k),
                                tr_t.arrive_time(day_t, ps_t.idx),
                                tr_t.total_price(k, ps_t.idx),
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
    if(!train.Get(id, tr) || !tr.released) {
        throw transaction_error("ticket not found");
    }
    if(num > tr.tot_seat) {
        throw transaction_error("exceed the total number of tickets");
    }
    pack.sidx = tr.search_station(st);
    if(!pack.sidx) {
        throw transaction_error("ticket not found");
    }
    pack.tidx = tr.search_station(tm, pack.sidx);
// if(id == "")
// std::cerr << "STATION_IDX " << pack.sidx << " " << pack.tidx << std::endl;
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
    bool fb = train.Get(id, tr);
    assert(fb);
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
    pnum.Clear();
    pass.Clear();
    return 0;
}

}