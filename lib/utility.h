#ifndef _TICKET_SYSTEM_UTILITY_H_
#define _TICKET_SYSTEM_UTILITY_H_

#include <cstring>
#include <cstdio>
#include <functional> // only for std::less<>
#include <cassert> // only for assert()
#include <cmath>
#include <iostream>
#include <utility>

#include "exception.h"
#include "datetime.h"
#include "str.h"
#include "TokenScanner.h"

// #define TICKSYS_DEBUG

#ifdef TICKSYS_DEBUG
#define ASSERT(__EXPR__) assert(__EXPR__)
#else
#define ASSERT(__EXPR__) 0
#endif

namespace ticket {

/**
 * @brief a pair structure
 */
template <typename _T1, typename _T2>
struct pair {
    _T1 first; 
    _T2 second;

    pair(): first(), second() {}
    template <typename _U1, typename _U2>
    pair(const _U1 &x, const _U2 &y): first(x), second(y) {}
    template <typename _U1, typename _U2>
    pair(const pair<_U1, _U2> &o): first(o.first), second(o.second) {}
    ~pair() = default;

};

template <typename _u1, typename _u2, typename _v1, typename _v2>
bool operator < (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
    if(lhs.first < rhs.first) return 1;
    if(rhs.first < lhs.first) return 0;
    return lhs.second < rhs.second;
}
template <typename _u1, typename _u2, typename _v1, typename _v2>
bool operator > (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
    if(lhs.first < rhs.first) return 0;
    if(rhs.first < lhs.first) return 1;
    return rhs.second < lhs.second;
}
template <typename _u1, typename _u2, typename _v1, typename _v2>
bool operator <= (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
    if(lhs.first < rhs.first) return 1;
    if(rhs.first < lhs.first) return 0;
    return !(rhs.second < lhs.second);
}
template <typename _u1, typename _u2, typename _v1, typename _v2>
bool operator >= (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
    if(lhs.first < rhs.first) return 0;
    if(rhs.first < lhs.first) return 1;
    return !(lhs.second < rhs.second);
}
template <typename _u1, typename _u2, typename _v1, typename _v2>
bool operator == (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}
template <typename _u1, typename _u2, typename _v1, typename _v2>
bool operator != (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
    return !(lhs.first == rhs.first && lhs.second == rhs.second);
}

template <typename T, typename U>
pair<T, U> make_pair(const T &a, const U &b) {
    return pair<T, U>(a, b);
}

/**
 * @brief implementation of sort function
 * 
 * use mergesort as the default sort algorithm
 * a default constructor of element type is required
 * 
 */

template <typename T, typename Cmp>
void mergesort(T *arr, int l, int r, T *tmp, Cmp comp) {
    if(l == r) return ;
    int mid = (l + r) >> 1;
    mergesort(arr, l, mid, tmp, comp);
    mergesort(arr, mid + 1, r, tmp, comp);
    int x = l, y = mid + 1, t = 0;
    while(x <= mid && y <= r) {
        tmp[t++] = comp(arr[x], arr[y])? arr[x++]: arr[y++];
    }
    while(x <= mid) tmp[t++] = arr[x++];
    while(y <= r) tmp[t++] = arr[y++];
    for(int i = 0; i < t; ++i) arr[i + l] = tmp[i];
}

template <typename T, typename Cmp>
void sort(T *arr, int l, int r) {
    if(r >= l) return ;
    T *tmp = new T [r - l]; Cmp comp;
    mergesort(arr, l, r - 1, tmp, comp);
    delete [] tmp;
}

/**
 * @brief implementation of cmin & cmax functions
 */

template <typename T, typename Cmp>
T& cmin(T& lhs, const T &rhs) {
    if(Cmp()(rhs, lhs)) lhs = rhs;
    return lhs;  
}

template <typename T, typename Cmp>
T& cmax(T& lhs, const T &rhs) {
    if(Cmp()(lhs, rhs)) lhs = rhs;
    return lhs;
}

// base of information pack classes 
struct InfoPack {};

/**
 * @brief Hash policy for Str type
 * based on the std::hash<std::string> policy
 * 
 */
struct StrHasher {
    template <size_t maxl>
    size_t operator () (const Str<maxl> &s) const {
        return std::hash<std::string>()(std::string(s));
    }
};
inline StrHasher strhasher;

/**
 * @brief Hash policy for pair type
 *  
 * @tparam _HashT Hash policy for the first type in pair 
 * @tparam _HashU Hash policy for the second type in pair
 */
template <
    typename T,
    typename U,
    typename _HashT = std::hash<T>,
    typename _HashU = std::hash<U>
>
struct PairHasher {
    size_t operator () (const pair<T, U> &obj) const {
        size_t key1 = _HashT()(obj.first);
        size_t key2 = _HashU()(obj.second);
        return key1 ^ (key2 + 0x9e3779b9 + (key1 << 6) + (key1 >> 2));
    }
};

}


#endif