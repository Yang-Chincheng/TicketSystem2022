
#include "train.h"

namespace ticket {

TrainInfo::TrainInfo(int _stanum, int _seatnum, Station *_sta, int *_price, const Time &_st_time, int *_tra_time, int *_stp_time, const Date &_st_date, const Date &_ed_date, char _type) 
{
    released = 0;
    type = _type;
    sta_num = _stanum;
    day_num = (_ed_date - _st_date) + 1;
    st_time = _st_time;
    st_date = _st_date, ed_date = _ed_date;
    tot_seat = _seatnum;
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

Time TrainInfo::arrive_time(int day, int idx) {
    if(day < 0) day += day_num;
    ASSERT(day >= 0 && day < day_num);
    ASSERT(idx >= 1);
    // if(idx < 0) idx += sta_num;
    Time time = st_time;
    time.date = st_date + day;
    time += arri_time[idx];
    return time;
}

Time TrainInfo::leave_time(int day, int idx) {
    if(day < 0) day += day_num;
    ASSERT(day >= 0 && day < day_num);
    ASSERT(idx >= 1);
    // if(idx < 0) idx += sta_num;
    Time time = st_time;
    time.date = st_date + day;
    time += leav_time[idx];
    return time;
}

int TrainInfo::total_price(int s, int t) {
    return price[t] - price[s];
}

int TrainInfo::search_station(const Station &des, int st, int ed) {
    if(ed == -1) ed = sta_num;
    for(int i = st; i <= ed; ++i) if(sta[i] == des) return i;
    return 0;
}

int SeatInfo::query_seat(int s, int t) {
    int res = 0x7fffffff;
    for(int i = s; i < t; ++i) res = std::min(res, seat[i]);
    return res;
}

int SeatInfo::modify_seat(int s, int t, int del) {
    for(int i = s; i < t; ++i) seat[i] += del;
    return 0;
}

std::ostream& operator << (std::ostream &os, const LinePack &pack) {
    
    return os;
}

std::ostream& operator << (std::ostream &os, const TravelPack &pack) {
    os << pack.id << " " << pack.strt << " " << pack.leav.date << " " << pack.leav << " -> ";
    os << pack.term << " " << pack.arri.date << " " << pack.arri << " ";
    os << pack.price << " " << pack.seat;
    return os;
}

int TrainManager::add_train(int opt_idx, const TrainID &id, int _stanum, int _seatnum, Station *_sta, int *_price, const Time &_st_time, int *_tra_time, int *_stp_time, const Date &_st_date, const Date &_ed_date, char _type) 
{
    if(train.count(id)) {
        throw train_error("train already exists, addition failed");
    }
    TrainInfo tr(
        _stanum, _seatnum, _sta, _price, _st_time, _tra_time, _stp_time, _st_date, _ed_date, _type
    );
    train.put(id, tr);
    // update seat info
    int day_num = tr.day_num;
    for(int i = 0; i < tr.day_num; ++i) { 
        seat.put(make_pair(id, i), SeatInfo(tr.sta_num, tr.tot_seat));
    }
    std::cout << "[" << opt_idx << "] 0" << std::endl;
    return 0;
}

int TrainManager::delete_train(int opt_idx, const TrainID &id) {
    TrainInfo tr;
    if(!train.get(id, tr)) {
        throw train_error("train not found");
    }
    if(tr.released) {
        throw train_error("the train has released, deletion failed");
    }
    train.remove(id);
    std::cout << "[" << opt_idx << "] 0" << std::endl;
    return 0;
}

int TrainManager::release_train(int opt_idx, const TrainID &id) {
    TrainInfo tr;
    if(!train.get(id, tr)) {
        throw train_error("train not found");
    }
    if(tr.released) {
        throw train_error("the train has released");
    }
    // set release tag as 1
    tr.released = 1;
    train.put(id, tr);

    // update by-pass train list
    // ===== TO-DO =====
    int hint = 1; 
    for(int i = 1; i <= tr.sta_num; ++i) {
        pass.put(
            make_pair(tr.sta[i], opt_idx),
            PassInfo(
                tr.leave_time(0, i).date, tr.leave_time(-1, i).date, id, i, hint
            )
        );
    }
    std::cout << "[" << opt_idx << "] 0" << std::endl;
    return 0;
}

int TrainManager::query_train(int opt_idx, const TrainID &id, const Date &date) {
    TrainInfo tr;
    SeatInfo st;
    if(!train.get(id, tr)) {
        throw train_error("train not found");
    }
    if(date < tr.st_date || date > tr.ed_date) {
        throw train_error("no schedule in this day");
    }
    int day = date - tr.st_date;
    bool fb = seat.get(make_pair(id, day), st);
    ASSERT(fb);

    std::cout << "[" << opt_idx << "] ";
    std::cout << id << " " << tr.type << std::endl;
    Time cur;
    for(int i = 1; i <= tr.sta_num; ++i) {
        // station name
        std::cout << tr.sta[i] << " ";
        // arriving time
        cur = tr.st_time, cur.date = date;
        cur += tr.arri_time[i];
        if(i == 1) std::cout << "xx-xx xx:xx";
        else std::cout << cur.date << " " << cur;
        std::cout << " -> ";
        // leaving time
        cur = tr.st_time, cur.date = date;
        cur += tr.leav_time[i];
        if(i == tr.sta_num) std::cout << "xx-xx xx:xx ";
        else std::cout << cur.date << " " << cur << " ";
        // accumulated price
        std::cout << tr.price[i] << " ";
        // number of rest seat
        if(i == tr.sta_num) std::cout << "x" << std::endl;
        else std::cout << st.seat[i] << std::endl;
    }
    return 0;
}

int TrainManager::query_ticket(int opt_idx, const Station &strt, const Station &term, const Date &date, bool cmp_type) {
    PassInfo ps;
    TrainInfo tr;
    SeatInfo st;
    // enumerate each train passing the start station
    auto iter = pass.lower_bound(make_pair(strt, 0));
    auto last = pass.lower_bound(make_pair(strt, opt_idx));
// if(opt_idx == 3543) {
//     std::cerr << "HERE " << (*iter).id << std::endl;
//     TrainInfo tr;
//     train.get((*iter).id, tr);
//     for(int i = 1; i <= tr.sta_num; ++i) std::cerr << tr.sta[i] << " ";
//     std::cerr << std::endl;
//     std::cerr << "HERE " << (*last).id << std::endl;
//     train.get((*last).id, tr);
//     for(int i = 1; i <= tr.sta_num; ++i) std::cerr << tr.sta[i] << " ";
//     std::cerr << std::endl;
// }
    vector<TravelPack> res; res.clear();
    for(; iter != last; ++iter) {
        ps = *iter;
        // rough check using abstract
        if(date >= ps.st_date && date <= ps.ed_date) {
            // get detail train info
            bool fb = train.get(ps.id, tr);
            ASSERT(fb);
            // detail check and get ans
            int sidx = ps.idx;
            int tidx = tr.search_station(term, sidx);
            if(tidx) {
                int day = date - ps.st_date;
                bool fb = seat.get(make_pair(ps.id, day), st);
                ASSERT(fb);
                int res_seat = st.query_seat(sidx, tidx);
                res.push_back(TravelPack(
                    ps.id, strt, term,
                    tr.leave_time(day, sidx), tr.arrive_time(day, tidx),
                    tr.total_price(sidx, tidx),
                    res_seat
                ));
            }
        }
    }
    // sort the answer
    if(cmp_type) sort<TravelPack, ByCost>(res, 0, res.size());
    else sort<TravelPack, ByTime>(res, 0, res.size());
    std::cout << "[" << opt_idx << "] " << res.size() << std::endl;
    for(auto &tick: res) std::cout << tick << std::endl;
    return 0;
}

int TrainManager::query_transfer(int opt_idx, const Station &strt, const Station &term, const Date &date, bool cmp_type) {
    bool tag = 0, fb;
    PassInfo ps_s, ps_t;
    TrainInfo tr_s, tr_t;
    SeatInfo st_s, st_t;
    hashmap<Station, int, StrHasher> mp;
    
    auto iter_s = pass.lower_bound(make_pair(strt, 0));
    auto last_s = pass.lower_bound(make_pair(strt, opt_idx));
    TransPack res;
    // enumerate all the train passing the start station
    for(; iter_s != last_s; ++iter_s) {
        ps_s = *iter_s;
//  ASSERT(iter_s == pass.Search(make_pair(strt, i)).first);
        if(date >= ps_s.st_date && date <= ps_s.ed_date) {
            fb = train.get(ps_s.id, tr_s);
            ASSERT(fb);
            int day_s = date - ps_s.st_date;
            // enumerate all the train passin the terminal station
            auto iter_t = pass.lower_bound(make_pair(term, 0));
            auto last_t = pass.lower_bound(make_pair(term, opt_idx));
            for(; iter_t != last_t; ++iter_t) {
                ps_t = *iter_t;
                // trains shound be different 
                if(ps_t.id == ps_s.id) continue;
                fb = train.get(ps_t.id, tr_t);
                ASSERT(fb);
                mp.clear();
                for(int k = ps_s.idx + 1; k <= tr_s.sta_num; ++k) mp[tr_s.sta[k]] = k;
                for(int k = ps_t.idx - 1; k >= 1; --k) {
                    // the exchange station
                    Station ex_sta = tr_t.sta[k];
                    int ex_idx = mp[ex_sta];
                    if(!ex_idx) continue;

                    // check route [tr1: sidx -> ex_idx] -> [tr2: k -> tidx]
                    Time arri_ex = tr_s.arrive_time(day_s, ex_idx);
                    if(arri_ex <= tr_t.leave_time(-1, k)) {
                        int day_t = std::max(0, arri_ex.date - tr_t.leave_time(0, k).date);
                        if(arri_ex > tr_t.leave_time(day_t, k)) day_t++;
                        
                        fb = seat.get(make_pair(ps_s.id, day_s), st_s);
                        ASSERT(fb);
                        fb = seat.get(make_pair(ps_t.id, day_t), st_t);
                        ASSERT(fb);
                        int res_s = st_s.query_seat(ps_s.idx, ex_idx);
                        int res_t = st_t.query_seat(k, ps_t.idx);
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
                            if(cmp_type) cmin<TransPack, ByCost>(res, cur);
                            else cmin<TransPack, ByTime>(res, cur);
                        }
                        else res = cur, tag = 1;

                    }
                }
            }
        }
    }
    std::cout << "[" << opt_idx << "] ";
    if(tag) std::cout << res.first << std::endl << res.second << std::endl;
    else std::cout << 0 << std::endl;
    return 0;
}

int TrainManager::clear_train() {
    train.clear();
    seat.clear();
    pass.clear();
    return 0;
}

}