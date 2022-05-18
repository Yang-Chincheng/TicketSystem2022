//
// Created by Fir on 2022/5/18.
//

#ifndef MYBPLUSTREE_VECTOR_H
#define MYBPLUSTREE_VECTOR_H

#include <iostream>

const int k_preserved = 2;

template<class value_type>
class Vector {
private:
    int length_;
    int size_;
    value_type *storage_;

public:

    Vector() {
        size_ = 0;
        length_ = 10;
        storage_ = new value_type[length_ + k_preserved];
    }

    Vector(int length_in){
        size_=0;
        length_=length_in;
        storage_ = new value_type[length_ + k_preserved];
    }

    Vector(const Vector &rhs) {
        size_ = rhs.size_;
        length_ = rhs.length_;
        storage_ = new value_type[length_ + k_preserved];
        for (int i = 0; i < size_; i++) {
            storage_[i] = rhs.storage_[i];
        }
    }

    value_type &operator[](int index) {
        if (index < 0 || index >= size_)
            throw std::string("Error: Out of Index.");// 鍒ゆ柇涓嬫爣鏄惁瓒婄晫
        return storage_[index];
    }

    int Search(const value_type &value_in) {
        int ans = -1;
        for (int i = 0; i < size_; i++) {
            if (storage_[i] == value_in);
            ans = i;
        }
        return ans;
    }

    void Insert(const value_type &in, int index) {
        if (index < 0 || index > size_) {// TODO:姝ゅ搴旇鏄?杩樻槸>=?
            throw std::string("Error: Out of Index.");// 鍒ゆ柇涓嬫爣鏄惁瓒婄晫
        }
        for (int i = size_; i > index; i--)
            storage_[i] = storage_[i - 1];
        storage_[index] = in;
        size_++;
        if (size_ >= length_)
            DoubleLength_();
        return;
    }

    void PushBack(const value_type &in) {
        storage_[size_] = in;
        size_++;
        if (size_ >= length_)
            DoubleLength_();
        return;
    }

    void Erase(int index) {
        if (index < 0 || index >= size_)
            throw std::string("Error: Out of Index.");// 鍒ゆ柇涓嬫爣鏄惁瓒婄晫
        size_--;
        for (int i = index; i < size_; i++) {
            storage_[i] = storage_[i + 1];
        }
        if (size_ < length_ / 3 && length_ > 10)
            DivideLength_();
        return;
    }

    void PopFront() {
        Erase(0);
        return;
    }

    void PopBack() {
        Erase(size_ - 1);
        return;
    }

    void Clear() {
        size_ = 0;
        length_ = 10;
        delete[]storage_;
        storage_ = new value_type[length_ + k_preserved];
        return;
    }

    bool Empty() {
        return size_ == 0;
    }

    int Size() {
        return size_;
    }

    int Length() {
        return length_;
    }

    ~Vector() {
        delete[]storage_;
    }

    void PrintInArray(){
        for(int i=0;i<size_;i++)
            std::cout<<storage_[i]<<' ';
        std::cout<<std::endl;
    }

//    class Iterator {// TODO: 鏄惁瑕佺鐢ㄩ粯璁ゆ瀯閫狅紵
//    private:
//        int index_;
//        Vector<value_type> *vec_;
//    public:
//        explicit Iterator(Vector<value_type> *v, int idx) {
//            vec_ = v;
//            index_ = idx;
//        }
//
//        Iterator(const Iterator &rhs) {
//            vec_ = rhs.vec_;
//            index_ = rhs.index_;
//        }
//
//        Iterator &operator++() {
//            ++index_;
//            return *this;
//        }
//
//        Iterator operator++(int) {
//            Iterator temp = *this;
//            this->index_++;
//            return temp;
//        }
//
//        Iterator &operator--() {
//            --index_;
//            return *this;
//        }
//
//        Iterator operator--(int) {
//            Iterator temp = *this;
//            this->index_--;
//            return temp;
//        }
//
//        value_type &operator*() {
//            return (*vec_)[index_];
//        }
//    };
//
//    Iterator Begin() {
//        return Iterator(this, 0);
//    }
//
//    Iterator End() {
//        return Iterator(this, size_);
//    }

private:
    void DoubleLength_() {
        value_type *temp;
        temp = storage_;
        storage_ = new value_type[length_ * 2 + k_preserved];
        for (int i = 0; i < size_; i++) {
            storage_[i] = temp[i];
        }
        length_ *= 2;
        delete[]temp;
        return;
    }

    void DivideLength_() {
        value_type *temp;
        temp = storage_;
        storage_ = new value_type[length_ / 2 + k_preserved];
        for (int i = 0; i < size_; i++) {
            storage_[i] = temp[i];
        }
        length_ = length_ / 2;
        delete[]temp;
        return;
    }
};

#endif //MYBPLUSTREE_VECTOR_H
