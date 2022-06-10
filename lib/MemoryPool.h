//
// Created by Fir on 2022/2/16.
//

#ifndef MYMEMORYPOOL_MEMORYPOOL_H
#define MYMEMORYPOOL_MEMORYPOOL_H

#include <iostream>

#include "FirVector.h"

const int k_head = sizeof(int) + 8;

template<class value_type>
class MemoryPool {
private:
    int elements_num_;
    Vector<value_type> storage_;
    std::fstream pool_;
    std::string pool_name_;

public:
    explicit MemoryPool(const std::string &name_in) {
        pool_name_ = name_in + std::string("_memory_pool_storage");
        storage_.Clear();
        pool_.open("./"+pool_name_);
        if (!pool_) {
            pool_.open("./"+pool_name_, std::ostream::out);
            elements_num_ = 0;
            pool_.close();
            pool_.open("./"+pool_name_);
        } else {
            pool_.seekg(0);
            pool_.read(reinterpret_cast<char *>(&elements_num_), sizeof(int));
            pool_.seekg(k_head);
            for (int i = 0; i < elements_num_; i++) {
                value_type temp;
                pool_.read(reinterpret_cast<char *>(&temp), sizeof(value_type));
                storage_.PushBack(temp);
            }
        }
    }

    ~MemoryPool() {
        elements_num_ = storage_.Size();
        pool_.seekp(0);
        pool_.write(reinterpret_cast<char *>(&elements_num_), sizeof(int));
        pool_.seekp(k_head);
        for (int i = 0; i < elements_num_; i++)
            pool_.write(reinterpret_cast<char *>(&storage_[i]), sizeof(value_type));
        storage_.Clear();
    }

    void Add(const value_type &value_in) {
        storage_.PushBack(value_in);
        elements_num_++;
    }

    value_type GetBack() {
        if (elements_num_ == 0)
            throw std::string("Error: No free space in this memory pool.");
        value_type ans;
        ans = storage_[elements_num_ - 1];
        storage_.PopBack();
        elements_num_--;
        return ans;
    }

    bool Empty() {
        return elements_num_ == 0;
    }

    void Clear() {
        elements_num_ = 0;
        storage_.Clear();
    }

    int Size() {
        return elements_num_;
    }

};


#endif //MYMEMORYPOOL_MEMORYPOOL_H
