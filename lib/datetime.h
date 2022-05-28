#ifndef _TICKET_SYSTEM_TIME_H_
#define _TICKET_SYSTEM_TIME_H_

#include <iostream>

namespace ticket {

/**
 * @brief a date manager
 * 
 * records date in a common year, where there's 28 days in Feb and 365 days in total
 * date info is formatted as mm-dd. (for example, August 17th -> 08-17)
 * support several date calculations and type conversions
 * 
 */

// numbers of days per month
const int __day_number[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
// total numbers of days per month in prefix order 
const int __prefix_total[13] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

struct Date {
    int month, day;
    Date() = default;
    Date(int mn, int dy): month(mn), day(dy) {}
    Date(const Date &o) = default;
    ~Date() = default;

    Date& operator += (int days);
    Date& operator -= (int days);
    Date  operator +  (int days);
    Date  operator -  (int days);


    operator std::string() const;

    friend std::ostream& operator << (std::ostream &os, const Date &date);

    friend bool operator == (const Date &lhs, const Date &rhs);
    friend bool operator != (const Date &lhs, const Date &rhs);
    friend bool operator <  (const Date &lhs, const Date &rhs);
    friend bool operator >  (const Date &lhs, const Date &rhs);
    friend bool operator <= (const Date &lhs, const Date &rhs);
    friend bool operator >= (const Date &lhs, const Date &rhs);

    friend int operator - (const Date &lhs, const Date &rhs);

};

/**
 * @brief a time manager, with date info attached
 * 
 * records a point of time, instead of a duration
 * date info is attached for time changes across the midnight
 * time info is formatted as hh:mm, and uses a 24-hour clock (for example, ten to two in the afternoon -> 13:50)
 * support several time calculations and type conversions
 * when used for display or conversion, date info is omitted
 *   
 */

struct Time {
    Date date;
    int hour, min;
    
    Time() = default;
    Time(int hr, int mi, int mn = 1, int dy = 1): date(mn, dy), hour(hr), min(mi) {}
    Time(const Time& o) = default;
    ~Time() = default;

    operator std::string() const;

    friend std::ostream& operator << (std::ostream& os, const Time& time);

    Time& operator += (int mins);
    Time& operator -= (int mins);
    Time  operator +  (int mins) const;
    Time  operator -  (int mins) const;

    friend bool operator == (const Time& lhs, const Time& rhs);
    friend bool operator != (const Time& lhs, const Time& rhs);
    friend bool operator <  (const Time& lhs, const Time& rhs);
    friend bool operator >  (const Time& lhs, const Time& rhs);
    friend bool operator <= (const Time& lhs, const Time& rhs);
    friend bool operator >= (const Time& lhs, const Time& rhs);

    friend int operator - (const Time& lhs, const Time& rhs);
};

}

#endif