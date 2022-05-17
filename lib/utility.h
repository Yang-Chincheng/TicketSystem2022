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
#include "time.h"
#include "str.h"

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

    template <typename _u1, typename _u2, typename _v1, typanem _v2>
    friend bool operator < (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
        if(lhs.first < rhs.first) return 1;
        if(rhs.first < lhs.first) return 0;
        return lhs.second < rhs.second;
    }
    template <typename _u1, typename _u2, typename _v1, typanem _v2>
    friend bool operator > (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
        if(lhs.first < rhs.first) return 0;
        if(rhs.first < lhs.first) return 1;
        return rhs.second < lhs.second;
    }
    template <typename _u1, typename _u2, typename _v1, typanem _v2>
    friend bool operator <= (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
        if(lhs.first < rhs.first) return 1;
        if(rhs.first < lhs.first) return 0;
        return lhs.second < rhs.second || lhs.second == rhs.second;
    }
    template <typename _u1, typename _u2, typename _v1, typanem _v2>
    friend bool operator >= (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
        if(lhs.first < rhs.first) return 0;
        if(rhs.first < lhs.first) return 1;
        return rhs.second < lhs.second || lhs.second == rhs.second;
    }
    template <typename _u1, typename _u2, typename _v1, typanem _v2>
    friend bool operator == (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
    template <typename _u1, typename _u2, typename _v1, typanem _v2>
    friend bool operator != (const pair<_u1, _u2> &lhs, const pair<_v1, _v2> &rhs) {
        return !(lhs.first == rhs.first && lhs.second == rhs.second);
    }


};

/**
 * @brief implementation of mergesort algorithm
 * 
 * a default constructor of element type is required
 * 
 */

template <typename T>
struct traits {
    using value_type = T::value_type;
};

template <typename T>
struct traits <T*> {
    using value_type = T;
};

template <
    typename T, 
    typename Cmp = std::less<typename traits<T>::value_type>
>
void mergesort(T arr, int l, int r, T tmp) {
    if(l >= r) return ;
    int mid = (l + r) >> 1;
    mergesort(arr, l, mid, tmp);
    mergesort(arr, mid + 1, r, tmp);
    int x = l, y = mid + 1, t = 0;
    while(x <= mid && y <= r) {
        tmp[t++] = (Cmp()(arr[x], arr[y])? arr[x++]: arr[y++]);
    }
    while(x <= mid) tmp[t++] = arr[x++];
    while(y <= r) tmp[t++] = arr[y++];
    for(int i = 0; i < t; ++i) arr[i + l] = tmp[i];
}

// base of information pack classes 
struct InfoPack {};

}


#endif