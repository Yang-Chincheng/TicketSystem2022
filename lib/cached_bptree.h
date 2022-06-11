#ifndef _TICKET_SYSTEM_CACHED_BPTREE_
#define _TICKET_SYSTEM_CACHED_BPTREE_

#include "BPlusTree.h"
#include "hashmap.h"
#include <functional>
#include <string>
#include <iostream>

namespace ticket {

template <
    typename _Key, 
    typename _Val, 
    typename _Hash = std::hash<_Key>,
    typename _Equal = std::equal_to<_Key>
>
class cached_bptree {
private:    
    int max_cache_num;
    BPTree<_Key, _Val> bpt;
    hashmap<_Key, _Val, _Hash, _Equal> cache;

    bool read(const _Key &key, _Val &val, int hint = -1) {
        if(cache.find(key) != cache.end()) val = cache[key];
        else {
            if(~hint) bpt.BiRead(hint, val);
            else if(!bpt.Get(key, val)) return 0;
            if(cache.size() == max_cache_num) cache.erase(cache.begin());
            cache[key] = val;
        }
        return 1;
    }
    void write(const _Key &key, const _Val &val, int opt_idx, bool rollback = 0) {
        if(cache.find(key) != cache.end()) cache[key] = val;
        else {
            if(cache.size() == max_cache_num) cache.erase(cache.begin());
            cache[key] = val;
        }
        bpt.Set(key, val, opt_idx, rollback);
    }

public:
    cached_bptree(const std::string &name, int _cache_size = 512): bpt(name), cache() {
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

        _Key getkey() {
            return iter.GetKey();
        }

        _Val getval() {
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

        int getpos() {
            return iter.StoragePosition();
        }
        
    };

    iterator begin() {
        return iterator(this, bpt.Begin());
    }

    iterator end() {
        return iterator(this, bpt.End());
    }

    bool get(const _Key &key, _Val &val) {
        return read(key, val);
    }
    bool get_with_hint(const _Key &key, _Val &val, int hint) {
        return read(key, val, hint);
    }
    void put(const _Key &key, const _Val &val, int opt_idx, bool rollback = 0) {
        write(key, val, opt_idx, rollback);
    }
    void remove(const _Key &key, int opt_idx, bool rollback = 0) {
        bpt.Delete(key, opt_idx, rollback);
        auto iter = cache.find(key);
        if(iter != cache.end()) cache.erase(iter);
    }

    iterator find(const _Key &key) {
        return iterator(this, bpt.Search(key).first);
    }
    bool count(const _Key &key) {
        if(cache.find(key) != cache.end()) return 1;
        return bpt.Search(key).second;
    }
    iterator lower_bound(const _Key &key) {
        return iterator(this, bpt.LowerBound(key));
    }

    bool empty() {
        return bpt.Empty();
    }

    void flush() {
        cache.clear();
    }

    void clear() {
        bpt.Clear(), cache.clear();
    }

    void roll_back(int tstamp) {
        cache.clear();
        bpt.RollBack(tstamp);
    }

    ~cached_bptree() {}
};

}

#endif