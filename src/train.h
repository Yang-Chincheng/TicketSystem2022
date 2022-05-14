#ifndef _TICKET_SYSTEM_TRAIN_H_
#define _TICKET_SYSTEM_TRAIN_H_

#include "../lib/utility.h"
#include "../test/bptree.h"
#include "../lib/hashmap.h"
#include "../lib/vector.h"
#include <iostream>

namespace ticket {

const int max_stanum = 102;
const int max_date = 100;

struct LineInfo {
    int sta_num;
    Station sta[max_stanum];
    int price[max_stanum];
    int arri_time[max_stanum];
    int leav_time[max_stanum];

    LineInfo() = default;
    LineInfo(
        int _stanum,  
        Station *_sta,
        int *_price,
        const Time &_st_time,
        int *_tra_time,
        int *_stp_time
    );
    LineInfo(const LineInfo &o) = default;
    ~LineInfo() = default;

};

struct LineInfoPack: public LineInfo {
    Time st_time;
    char type;
    int seat[max_stanum];
    LineInfoPack() = default;
    LineInfoPack(
        const LineInfo &info, 
        const Time &_st_time, 
        char _type, 
        int *_seat
    ): LineInfo(info), st_time(_st_time) {
        type = _type;
        for(int i = 1; i < info.sta_num; ++i) seat[i] = _seat[i];
    }
    friend std::ostream& operator << (std::ostream &os, const LineInfoPack &info);
};

struct TrainInfo {
    bool released;
    char type;
    int sta_num; // 1-base
    int day_num; // 0-base
    Time st_time;
    Date st_date, ed_date;
    LineInfo line;
    int seat[max_date][max_stanum];

    TrainInfo() = default;
    TrainInfo(
        int _stanum,  
        int _seatnum,
        Station *_sta,
        int *_price,
        const Time &_st_time,
        int *_tra_time,
        int *_stp_time,
        const Date &_st_date,
        const Date &_ed_date,
        char _type
    );
    TrainInfo(const TrainInfo &o) = default;
    ~TrainInfo() = default;

    Time arrive_time(int day, int idx);
    Time leave_time(int day, int idx);
    int total_price(int s, int t);
    int rest_seat(int d, int s, int t);
    int search_station(const Station &sta, int st, int ed);

};

struct TravelInfoPack {
    TrainID id;
    Station strt, term;
    Time leav, arri;
    int price;
    int seat;

    TravelInfoPack() = default;
    TravelInfoPack(
        const TrainID &_id,
        const Station &_strt, const Station &_term, 
        const Time &_leav, const Time &_arri,
        int _price, int _seat
    ): id(_id), strt(_strt), term(_term), leav(_leav), arri(_arri) {
        price = _price, seat = _seat;
    }

    friend std::ostream& operator << (std::ostream &os, const TravelInfoPack &pack);

};

using TransInfoPack = pair<TravelInfoPack, TravelInfoPack>;


const int max_pnum = 10000;

struct PassTrain {
    struct MetaData {
        Date st_date, ed_date;
        TrainID id;
        int idx;
        bptree<TrainID, TrainInfo>::iterator iter;
    };
    int pnum;
    MetaData ptrain[max_pnum];
};

class TrainManager {
private:
    bptree<TrainID, TrainInfo> train;
    bptree<TrainID, PassTrain> station;

public:
    TrainManager(): train("train"), station("station") {}
    TrainManager(const TrainManager &o) = delete;
    ~TrainManager() = default;

    int add_train(
        const TrainID &id,
        int _stanum,  
        int _seatnum,
        Station *_sta,
        int *_price,
        const Time &_st_time,
        int *_tra_time,
        int *_stp_time,
        const Date &_st_date,
        const Date &_ed_date,
        char _type
    );

    int delete_train(const TrainID& id);

    int release_train(const TrainID &id);
    
    int query_train(
        const TrainID& id, 
        const Date &date,
        LineInfoPack &Dinfo
    );

    template <typename _Cmp>
    int query_ticket(
        const Station &strt,
        const Station &term,
        const Date &date,
        vector<TravelInfoPack> &Tinfo
    );

    template <typename _Cmp>
    int query_transfer(
        const Station &strt,
        const Station &term,
        const Date &date,
        TransInfoPack &Tinfo
    );

    static int query_seat(
        const TrainInfo &tr,
        const Date &date, 
        const Station &strt,
        const Station &term,
        TravelInfoPack &info,
        const TrainID &id
    );

    int modify_seat(
        const TrainID &id,
        const Date &date, 
        const Station &strt,
        const Station &term,
        int delta
    );

};

}

#endif