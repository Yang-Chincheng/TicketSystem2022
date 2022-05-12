#ifndef _TICKET_SYSTEM_STR_H_
#define _TICKET_SYSTEM_STR_H_

#include <iostream>
#include <cstring>
#include <cassert>
#include <functional>
#include "exception.h"

namespace ticket {

/**
 * @brief an encapsulation of C-style string (char array with fixed length)
 * @tparam maxlen: maxinum length of the string
 * 
 * 
 */

template <size_t maxlen>
class Str {
private:
    size_t len;
    char str[maxlen + 2];

public:
    Str(): len(0) {str[0] = '\0'; }
    Str(const char *_str) {
        len = strlen(_str);
        assert(len <= maxlen);
        memcpy(str, _str, len * sizeof(char));
        str[len] = '\0';
    }
    Str(const std::string &_str) {
        len = _str.length();
        assert(len <= maxlen);
        memcpy(str, _str.c_str(), len * sizeof(char));
        str[len] = '\0';
    }
    template <size_t maxl>
    Str(const Str<maxl> &_str) {
        assert(_str.len <= maxlen);
        len = _str.len;
        memcpy(str, _str.c_str(), len * sizeof(char));
        str[len] = '\0';
    }
    ~Str() = default;

    size_t length() const {return len; }
    const char* c_str() const {return str; }

    operator bool() const {return len != 0; }
    operator std::string() const {return std::string(str); }

    char& operator [] (size_t idx) {
        assert(idx >= 0 && idx < maxlen);
        return str[idx];
    }
    const char& operator [] (size_t idx) const {
        assert(idx >= 0 && idx < maxlen);
        return str[idx];
    }

    friend std::istream& operator >> (std::istream& is, Str &obj) {
        std::string buff; is >> buff;
        assert(buff.length() <= maxlen);
        obj.len = buff.length();
        memcpy(obj.str, buff.c_str(), obj.len * sizeof(char));
        obj.str[obj.len] = '\0';
        return is;
    }
    friend std::ostream& operator << (std::ostream& os, const Str &obj) {
        os << obj.str; return os;
    }

};


template <size_t maxl1, size_t maxl2>
bool operator == (const Str<maxl1> &lhs, const Str<maxl2> &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}
template <size_t maxl>
bool operator == (const Str<maxl> &lhs, const char *rhs) {
    return strcmp(lhs.c_str(), rhs) == 0;
}
template <size_t maxl>
bool operator == (const Str<maxl> &lhs, const std::string &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

template <size_t maxl1, size_t maxl2>
bool operator != (const Str<maxl1> &lhs, const Str<maxl2> &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) != 0;
}
template <size_t maxl>
bool operator != (const Str<maxl> &lhs, const char *rhs) {
    return strcmp(lhs.c_str(), rhs) != 0;
}
template <size_t maxl>
bool operator != (const Str<maxl> &lhs, const std::string &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) != 0;
}

template <size_t maxl1, size_t maxl2>
bool operator < (const Str<maxl1> &lhs, const Str<maxl2> &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}
template <size_t maxl>
bool operator < (const Str<maxl> &lhs, const char *rhs) {
    return strcmp(lhs.c_str(), rhs) < 0;
}
template <size_t maxl>
bool operator < (const Str<maxl> &lhs, const std::string &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

template <size_t maxl1, size_t maxl2>
bool operator <= (const Str<maxl1> &lhs, const Str<maxl2> &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) <= 0;
}
template <size_t maxl>
bool operator <= (const Str<maxl> &lhs, const char *rhs) {
    return strcmp(lhs.c_str(), rhs) <= 0;
}
template <size_t maxl>
bool operator <= (const Str<maxl> &lhs, const std::string &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) <= 0;
}

template <size_t maxl1, size_t maxl2>
bool operator > (const Str<maxl1> &lhs, const Str<maxl2> &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) > 0;
}
template <size_t maxl>
bool operator > (const Str<maxl> &lhs, const char *rhs) {
    return strcmp(lhs.c_str(), rhs) > 0;
}
template <size_t maxl>
bool operator > (const Str<maxl> &lhs, const std::string &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

template <size_t maxl1, size_t maxl2>
bool operator >= (const Str<maxl1> &lhs, const Str<maxl2> &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) >= 0;
}
template <size_t maxl>
bool operator >= (const Str<maxl> &lhs, const char *rhs) {
    return strcmp(lhs.c_str(), rhs) >= 0;
}
template <size_t maxl>
bool operator >= (const Str<maxl> &lhs, const std::string &rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) >= 0;
}

struct StrHasher {
    template <size_t maxl>
    size_t operator () (const Str<maxl> &s) const {
        return std::hash<std::string>()(std::string(s));
    }
};

const size_t usr_len = 20;
const size_t psw_len = 30;
const size_t name_len = 20;
const size_t maddr_len = 30;
const size_t trID_len = 20;
const size_t sta_len = 40;
const size_t cmd_len = 20;

using Username = Str<usr_len>;
using Password = Str<psw_len>;
using Name = Str<name_len>;
using MailAddr = Str<maddr_len>;
using trainID = Str<trID_len>;
using Station = Str<sta_len>;
using Command = Str<cmd_len>;

}

#endif