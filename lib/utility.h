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

};

/**
 * @brief implementation of mergesort algorithm
 * 
 * a default constructor of element type is required
 * 
 */

template <typename T, typename Cmp = std::less<T>>
void _sort_func(T *arr, int l, int r, T* tmp, Cmp comp) {
    if(l == r) return ;
    int mid = (l + r) >> 1;
    _sort_func(arr, l, mid, tmp, comp);
    _sort_func(arr, mid + 1, r, tmp, comp);
    int x = l, y = mid + 1, t = 0;
    while(x <= mid && y <= r) {
        tmp[t++] = (comp(arr[x], arr[y])? arr[x++]: arr[y++]);
    }
    while(x <= mid) tmp[t++] = arr[x++];
    while(y <= r) tmp[t++] = arr[y++];
    for(int i = 0; i < t; ++i) tmp[i + l] = arr[i];
}

template <typename T, typename Cmp>
void mergesort(T *arr, int l, int r, Cmp comp) {
    T tmp = new T[r - l + 1];
    _sort_func(arr, l, r, tmp, comp);
    delete [] tmp;
}

}


#endif