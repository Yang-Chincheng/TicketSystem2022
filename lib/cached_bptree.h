#ifndef _TICKET_SYSTEM_CACHED_BPTREE_
#define _TICKET_SYSTEM_CACHED_BPTREE_

#include "BPlusTree.h"
#include "hashmap.h"
#include <functional>
#include <string>
#include <iostream>
// #include <map>

namespace ticket {

template <
    typename _Key, 
    typename _Val, 
    typename _Hash = std::hash<_Key>,
    typename _Equal = std::equal_to<_Key>
>
class cached_bptree {
// public:
//     struct iterator;
//     friend struct iterator;

private:    
    int max_cache_num;
    int cache_num;
    BPTree<_Key, _Val> bpt;
    hashmap<_Key, _Val, _Hash, _Equal> cache;

    bool read(const _Key &key, _Val &val, int hint = -1) {
        if(cache.find(key) != cache.end()) val = cache[key];
        else {
            if(~hint) bpt.BiRead(hint, val);
            else if(!bpt.Get(key, val)) return 0;
            if(cache_num == max_cache_num) {
                auto cur = cache.begin();
                bpt.Set(cur->first, cur->second);
                cache.erase(cur);
                cache_num--;
            }
            cache[key] = val;
            cache_num++;
        }
        return 1;
    }
    void write(const _Key &key, const _Val &val) {
        if(cache.find(key) != cache.end()) cache[key] = val;
        else {
            if(cache_num == max_cache_num) {
                auto cur = cache.begin();
                bpt.Set(cur->first, cur->second);
                cache.erase(cur);
                cache_num--;
            }
            cache[key] = val;
            cache_num++;
        }
        bpt.Set(key, val);
    }
    void erase(const _Key &key) {
        auto iter = cache.find(key);
        if(iter == cache.end()) return ;
        cache.erase(iter);
        cache_num--;
    }

public:
    cached_bptree(const std::string &name, int _cache_size = 512): bpt(name), cache() {
        cache_num = 0;
        max_cache_num = std::max((_cache_size << 10) / (sizeof(_Key) + sizeof(_Val)), 1ul);
    }
    
    struct iterator {
        cached_bptree *tree;
        typename BPTree<_Key, _Val>::Iterator iter;

        iterator() = default;
        iterator(cached_bptree *_tree, const typename BPTree<_Key, _Val>::Iterator &_iter)
        : tree(_tree), iter(_iter) {}
        iterator(const iterator &o) = default;
        ~iterator() = default;

        bool operator == (const iterator &rhs) const {
            return tree == rhs.tree && iter == rhs.iter;
        }
        bool operator != (const iterator &rhs) const {
            return tree != rhs.tree || iter != rhs.iter;
        }

        _Val operator * () {
            _Val ret;
            tree->read(iter.GetKey(), ret, iter.StoragePosition());
            return ret;
        }

        iterator& operator ++ () {
            ++iter; return *this;
        }
        iterator& operator -- () {
            --iter; return *this;
        }

        int position() {
            return iter.StoragePosition();
        }
        
    };

    bool get(const _Key &key, _Val &val) {
        return read(key, val);
    }
    bool get_with_hint(const _Key &key, _Val &val, int hint) {
        return read(key, val, hint);
    }
    void put(const _Key &key, const _Val &val) {
        write(key, val);
    }
    void remove(const _Key &key) {
        bpt.Delete(key), erase(key);
    }

    iterator find(const _Key &key) {
        return iterator(this, bpt.Search(key).first);
    }
    bool count(const _Key &key) {
        return bpt.Search(key).second;
    }
    iterator lower_bound(const _Key &key) {
        return iterator(this, bpt.LowerBound(key));
    }

    // void flush() {
    //     for(auto &cur: cache) {
    //         bpt.Set(cur.first, cur.second);
    //     }
    //     cache.clear();
    //     cache_num = 0;
    // }

    bool empty() {
        return bpt.Empty();
    }

    void clear() {
        bpt.Clear(), cache.clear();
    }

    ~cached_bptree() {}
};

}

#endif