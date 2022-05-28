#include "datetime.h"


namespace ticket {

/**
 * @brief implementaion of struct Date; 
 */
Date& Date::operator += (int days) {
    day += days;
    while(day > __day_number[month]) day -= __day_number[month++];
    return *this;
}
Date& Date::operator -= (int days) {
    day -= days;
    while(day <= 0) day += __day_number[--month];
    return *this;
}
Date Date::operator + (int days) {
    Date ret(*this); ret += days;
    return ret;
}
Date Date::operator - (int days) {
    Date ret(*this); ret -= days;
    return ret; 
}

Date::operator std::string() const {
    std::string ret = "";
    ret = char('0' + month % 10) + ret;
    ret = char('0' + month / 10) + ret;
    ret = ret + '-';
    ret = ret + char('0' + day / 10);
    ret = ret + char('0' + day % 10);
    return ret;
}

std::ostream& operator << (std::ostream &os, const Date &date) {
    os << date.month / 10 << date.month % 10; 
    os << '-';
    os << date.day / 10 << date.day % 10;
    return os;
}

bool operator == (const Date &lhs, const Date &rhs) {
    return lhs.month == rhs.month && lhs.day == rhs.day;
}
bool operator != (const Date &lhs, const Date &rhs) {
    return lhs.month != rhs.month || lhs.day != rhs.day;
}
bool operator < (const Date &lhs, const Date &rhs) {
    if(lhs.month != rhs.month) return lhs.month < rhs.month;
    return lhs.day < rhs.day;
}
bool operator > (const Date &lhs, const Date &rhs) {
    if(lhs.month != rhs.month) return lhs.month > rhs.month;
    return lhs.day > rhs.day;
}
bool operator <= (const Date &lhs, const Date &rhs) {
    if(lhs.month != rhs.month) return lhs.month < rhs.month;
    return lhs.day <= rhs.day;
}
bool operator >= (const Date &lhs, const Date &rhs) {
    if(lhs.month != rhs.month) return lhs.month > rhs.month;
    return lhs.day >= rhs.day;
}
int operator - (const Date &lhs, const Date &rhs) {
    int days = __prefix_total[lhs.month - 1] - __prefix_total[rhs.month - 1];
    return days + lhs.day - rhs.day;
}

/**
 * @brief implementation of struct Time;  
 */

Time::operator std::string() const {
    std::string ret = "";
    ret += '0' + hour / 10;
    ret += '0' + hour % 10;
    ret += ':';
    ret += '0' + min / 10;
    ret += '0' + min % 10;
    return ret;
}

std::ostream& operator << (std::ostream& os, const Time& time) {
    os << time.hour / 10 << time.hour % 10;
    os << ':';
    os << time.min / 10 << time.min % 10;
    return os;
}

Time& Time::operator += (int mins) {
    min += mins;
    if(min >= 60) {
        hour += min / 60, min %= 60;
        if(hour >= 24) date += hour / 24, hour %= 24;
    }
    return *this;
}
Time& Time::operator -= (int mins) {
    min -= mins;
    if(min < 0) {
        int tmp = (-min + 59) / 60;
        hour -= tmp, min += 60 * tmp;
        if(hour < 0) {
            tmp = (-hour + 23) / 24;
            date -= tmp, hour += 24 * tmp;
        }
    }
    return *this;
}

Time Time::operator + (int mins) const {
    Time ret(*this); ret += mins;
    return ret;
}
Time Time::operator - (int mins) const {
    Time ret(*this); ret -= mins;
    return ret;
}

bool operator == (const Time& lhs, const Time& rhs) {
    return lhs.hour == rhs.hour && lhs.min == rhs.min && lhs.date == rhs.date;
}
bool operator != (const Time& lhs, const Time& rhs) {
    return lhs.hour != rhs.hour || lhs.min != rhs.min || lhs.date != rhs.date;
}
bool operator < (const Time& lhs, const Time& rhs) {
    if(lhs.date != rhs.date) return lhs.date < rhs.date;
    if(lhs.hour != rhs.hour) return lhs.hour < rhs.hour;
    return lhs.min < rhs.min;
}
bool operator > (const Time& lhs, const Time& rhs) {
    if(lhs.date != rhs.date) return lhs.date > rhs.date;
    if(lhs.hour != rhs.hour) return lhs.hour > rhs.hour;
    return lhs.min > rhs.min;
}
bool operator <= (const Time& lhs, const Time& rhs) {
    if(lhs.date != rhs.date) return lhs.date < rhs.date;
    if(lhs.hour != rhs.hour) return lhs.hour < rhs.hour;
    return lhs.min <= rhs.min;
}
bool operator >= (const Time& lhs, const Time& rhs) {
    if(lhs.date != rhs.date) return lhs.date > rhs.date;
    if(lhs.hour != rhs.hour) return lhs.hour > rhs.hour;
    return lhs.min >= rhs.min;
}

int operator - (const Time& lhs, const Time& rhs) {
    int mins = (lhs.date - rhs.date) * 1440;
    mins += (lhs.hour - rhs.hour) * 60;
    return mins + lhs.min - rhs.min;
}

}