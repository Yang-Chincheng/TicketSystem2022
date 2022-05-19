//
// Created by Fir on 2022/2/6.
// This is a BPlusTree written for the Ticket System.
//

#ifndef MYBPLUSTREE_BPLUSTREE_H
#define MYBPLUSTREE_BPLUSTREE_H

#include <iostream>
#include <string>
#include <fstream>

#include "MemoryPool.h"

const int k_max_size = 10;
const int k_min_size = 3;
const int k_head_preserved = 10 * sizeof(int);

// Note: BPTree的特性决定了，叶节点的key和value个数相同，而内节点的number(key)=number(value)-1
// Note: 在此实现中，内节点 key(i-1) <= i号子块中的key值 < key(i)

template<class key_type, class value_type>
class BPTree {
private:
    struct key_address_pair {
        key_type key;
        int address;

        key_address_pair() {}

        key_address_pair(key_type key_in, int address_in) : key(key_in) {
            address = address_in;
        }

        ~key_address_pair() {
        }

        bool operator==(const key_address_pair &rhs) const {
            return key == rhs.key;
        }

        bool operator!=(const key_address_pair &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const key_address_pair &rhs) const {
            return key < rhs.key;
        }

        bool operator>(const key_address_pair &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const key_address_pair &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const key_address_pair &rhs) const {
            return !(*this < rhs);
        }
    };

    class Node_ {
    public:
        key_address_pair data[k_max_size + k_min_size];
        // 非叶结点时，data储存子节点地址
        // 叶结点时，data存储数据在另一个文件中的地址
        int prev_num = -1;
        int next_num = -1;
        int my_num = -1;
        int parent_num = -1;
        int elements_num = 0;
        bool if_leaf = true;

        Node_() {
        }

        Node_(int num_in) {
            my_num = num_in;
        }

        Node_(int num_in, int prev_in, int next_in) {
            prev_num = prev_in;
            next_num = next_in;
            my_num = num_in;
        }

        ~Node_() {
        }

//        void Add(key_address_pair data_in) {
//            // 本层不考虑裂块
//            int left = 0;
//            int right = elements_num - 1;
//            int mid, pos;
//            while (left <= right) {
//                mid = (left + right) / 2;
//                if (data[mid] == data_in)
//                    throw std::string("Error: Already in this tree.");
//                if (data[mid] > data_in)
//                    right = mid - 1;
//                else
//                    left = mid + 1;
//            }
//            pos = left;
//            for (int i = elements_num; i > pos; i--) {
//                data[i] = data[i - 1];
//            }
//            data[pos] = data_in;
//            elements_num++;
//        }


//        void Delete(key_type key_in) {
//            // 本层不考虑并块
//            int left = 0;
//            int right = elements_num - 1;
//            int mid, pos;
//            bool if_find = false;
//            while (left <= right && !if_find) {
//                mid = (left + right) / 2;
//                if (data[mid].key == key_in)
//                    if_find = true;
//                else if (data[mid].key > key_in)
//                    right = mid - 1;
//                else
//                    left = mid + 1;
//            }
//            if (!if_find)
//                throw std::string("Error: Don't contain in this tree.");
//            pos = mid;
//            for (int i = pos; i < elements_num - 1; i++) {
//                data[i] = data[i + 1];
//            }
//            elements_num--;
//        }

        key_address_pair &operator[](int num_in) {
            if (num_in < 0 && num_in >= elements_num)
                throw std::string("Error: Out of range.");
            return data[num_in];
        }

        bool operator==(const Node_ &rhs) const {
            return my_num == rhs.my_num;
        }

        bool operator!=(const Node_ &rhs) const {
            return !(rhs == *this);
        }
    };


    int node_num_; // 所有已经开过的块数量，只增不减
    int data_num_; // 所有已经开过的空间数量，只增不减
    int record_num_; // 内部存储的记录数量
    int height_; // B+树的高度
    std::string index_name_;
    std::string data_name_;
    std::fstream index_;
    std::fstream data_;
    int root_num_;
    Node_ root_;
    MemoryPool<int> node_memory_pool_;
    MemoryPool<int> data_memory_pool_;

public:
    explicit BPTree(const std::string &name_in) : node_memory_pool_(name_in + std::string("_index")),
                                                  data_memory_pool_(name_in + std::string("_data")) {
        index_name_ = name_in + std::string("_index_storage");
        data_name_ = name_in + std::string("_data_storage");
        index_.open(index_name_);
        if (!index_) {
            index_.open(index_name_, std::ostream::out);
            node_num_ = 0;
            data_num_ = 0;
            record_num_ = 0;
            height_ = 1;
            Node_ root(node_num_);
            root_ = root;
            root_num_ = root_.my_num;
            node_num_++;
            index_.seekp(k_head_preserved + sizeof(Node_) * root_num_);
            index_.write(reinterpret_cast<char *>(&root_), sizeof(Node_));
            index_.close();
            index_.open(index_name_);
            data_.open(data_name_, std::ostream::out);
            data_.close();
            data_.open(data_name_);
        } else {
            index_.seekg(0);
            index_.read(reinterpret_cast<char *>(&node_num_), sizeof(int));
            index_.read(reinterpret_cast<char *>(&data_num_), sizeof(int));
            index_.read(reinterpret_cast<char *>(&record_num_), sizeof(int));
            index_.read(reinterpret_cast<char *>(&height_), sizeof(int));
            index_.read(reinterpret_cast<char *>(&root_num_), sizeof(int));
            index_.seekp(k_head_preserved + sizeof(Node_) * root_num_);
            index_.read(reinterpret_cast<char *>(&root_), sizeof(Node_));
            data_.open(data_name_);
        }
    }

    ~BPTree() {
        index_.seekp(0);
        index_.write(reinterpret_cast<char *>(&node_num_), sizeof(int));
        index_.write(reinterpret_cast<char *>(&data_num_), sizeof(int));
        index_.write(reinterpret_cast<char *>(&record_num_), sizeof(int));
        index_.write(reinterpret_cast<char *>(&height_), sizeof(int));
        index_.write(reinterpret_cast<char *>(&node_num_), sizeof(int));
        index_.seekp(k_head_preserved + sizeof(Node_) * node_num_);
        index_.write(reinterpret_cast<char *>(&root_), sizeof(Node_));
        index_.close();
        data_.close();
    }

    bool Empty() {
        return root_.elements_num == 0;
    }

    void Clear() {
        index_.open(index_name_, std::ostream::out);
        index_.close();
        index_.open(index_name_);
        data_.open(data_name_, std::ostream::out);
        data_.close();
        data_.open(data_name_);
        node_memory_pool_.Clear();
        data_memory_pool_.Clear();
        node_num_ = 0;
        data_num_ = 0;
        record_num_ = 0;
        height_ = 1;
        Node_ root(node_num_);
        root_ = root;
        root_num_ = root_.my_num;
        node_num_++;
        index_.seekp(k_head_preserved + sizeof(Node_) * root_num_);
        index_.write(reinterpret_cast<char *>(&root_), sizeof(Node_));
    }

    int Size() {
        return record_num_;
    }

    int Height() {
        return height_;
    }

    bool Search(key_type key_in) {
        Node_ object_node= FindObjectNode_(key_in);
//        index_.seekg(k_head_preserved + sizeof(Node_) * root_.my_num);
//        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        while (!object_node.if_leaf) {
//            // 找到叶节点
//            int l, r, mid;
//            l = 0;
//            r = object_node.elements_num - 2;
//            while (l <= r) {
//                mid = (l + r) / 2;
//                if (object_node.data[mid].key > key_in)
//                    r = mid - 1;
//                else
//                    l = mid + 1;
//            }
//            int pos = l;
//            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[pos].address);
//            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        }
        // 找到节点内是否有对应的
        int l, r, mid;
        l = 0;
        r = object_node.elements_num - 1;
        bool find = false;
        while (!find && l <= r) {
            mid = (l + r) / 2;
            if (object_node.data[mid].key == key_in)
                find = true;
            if (object_node.data[mid].key > key_in)
                r = mid - 1;
            else
                l = mid + 1;
        }
        return find;
    }

    bool Get(key_type key_in, value_type &value_in) {
        Node_ object_node= FindObjectNode_(key_in);
//        index_.seekg(k_head_preserved + sizeof(Node_) * root_.my_num);
//        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        while (!object_node.if_leaf) {
//            // 找到叶节点
//            int l, r, mid;
//            l = 0;
//            r = object_node.elements_num - 2;
//            while (l <= r) {
//                mid = (l + r) / 2;
//                if (object_node.data[mid].key > key_in)
//                    r = mid - 1;
//                else
//                    l = mid + 1;
//            }
//            int pos = l;
//            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[pos].address);
//            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        }
        // 找到节点内是否有对应的
        int l, r, mid, pos;
        l = 0;
        r = object_node.elements_num - 1;
        bool find = false;
        while (!find && l <= r) {
            mid = (l + r) / 2;
            if (object_node.data[mid].key == key_in)
                find = true, pos = mid;
            if (object_node.data[mid].key > key_in)
                r = mid - 1;
            else
                l = mid + 1;
        }
        if (find) {
            data_.seekg(sizeof(value_type) * object_node.data[pos].address);
            data_.read(reinterpret_cast<char *>(&value_in), sizeof(value_type));
        }
        return find;
    }

    void Insert(key_type key_in, value_type value_in) {
        Node_ object_node= FindObjectNode_(key_in);
//        index_.seekg(k_head_preserved + sizeof(Node_) * root_.my_num);
//        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        while (!object_node.if_leaf) {
//            // 找到叶节点
//            int l, r, mid;
//            l = 0;
//            r = object_node.elements_num - 2;
//            while (l <= r) {
//                mid = (l + r) / 2;
//                if (object_node.data[mid].key > key_in)
//                    r = mid - 1;
//                else
//                    l = mid + 1;
//            }
//            int pos = l;
//            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[pos].address);
//            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        }
        // 找到节点内的对应位置进行插入
        int l, r, mid, pos;
        l = 0;
        r = object_node.elements_num - 1;
        while (l <= r) {
            mid = (l + r) / 2;
            if (object_node.data[mid].key == key_in)
                throw std::string("Error: Element already in this tree.");
            if (object_node.data[mid].key > key_in)
                r = mid - 1;
            else
                l = mid + 1;
        }
        pos = l;
        for (int i = object_node.elements_num; i > pos; i--)
            object_node.data[i] = object_node.data[i - 1];
        object_node.data[pos].key = key_in;
        if (data_memory_pool_.Empty()) {
            object_node.data[pos].address = data_num_;
            data_num_++;
        } else {
            object_node.data[pos].address = data_memory_pool_.GetBack();
        }
        object_node.elements_num++;
        record_num_++;
        // 向外存进行记入
        data_.seekp(sizeof(value_type) * object_node.data[pos].address);
        data_.write(reinterpret_cast<char *>(&value_in), sizeof(value_type));
        index_.seekp(k_head_preserved + sizeof(Node_) * object_node.my_num);
        index_.write(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        // 检查是否需要裂块
        if (object_node.elements_num >= k_max_size)
            BreakNode_(object_node);
        if (object_node == root_)
            root_ = object_node;
    }

    void Set(key_type key_in, value_type value_in) {
        Node_ object_node= FindObjectNode_(key_in);
//        index_.seekg(k_head_preserved + sizeof(Node_) * root_.my_num);
//        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        while (!object_node.if_leaf) {
//            // 找到叶节点
//            int l, r, mid;
//            l = 0;
//            r = object_node.elements_num - 2;
//            while (l <= r) {
//                mid = (l + r) / 2;
//                if (object_node.data[mid].key > key_in)
//                    r = mid - 1;
//                else
//                    l = mid + 1;
//            }
//            int pos = l;
//            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[pos].address);
//            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        }
        int l, r, mid, pos;
        l = 0;
        r = object_node.elements_num - 1;
        bool find = false;
        while (!find && l <= r) {
            mid = (l + r) / 2;
            if (object_node.data[mid].key == key_in)
                find = true, pos = mid;
            if (object_node.data[mid].key > key_in)
                r = mid - 1;
            else
                l = mid + 1;
        }
        if(!find){
            Insert(key_in,value_in);
            return;
        }
        data_.seekp(sizeof(value_type) * object_node.data[pos].address);
        data_.write(reinterpret_cast<char *>(&value_in), sizeof(value_type));
    }

    void Delete(key_type key_in, value_type value_in) {
        Delete(key_in);
    }

    void Delete(key_type key_in) {
        Node_ object_node= FindObjectNode_(key_in);
//        index_.seekg(k_head_preserved + sizeof(Node_) * root_.my_num);
//        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        while (!object_node.if_leaf) {
//            // 找到叶节点
//            int l, r, mid;
//            l = 0;
//            r = object_node.elements_num - 2;
//            while (l <= r) {
//                mid = (l + r) / 2;
//                if (object_node.data[mid].key > key_in)
//                    r = mid - 1;
//                else
//                    l = mid + 1;
//            }
//            int pos = l;
//            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[pos].address);
//            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
//        }
        // 找到节点内的对应项进行删除
        int l, r, mid, pos;
        bool if_find = false;
        l = 0;
        r = object_node.elements_num - 1;
        while (l <= r && !if_find) {
            mid = (l + r) / 2;
            if (object_node.data[mid].key == key_in) {
                pos = mid;
                if_find = true;
            } else if (object_node.data[mid].key > key_in)
                r = mid - 1;
            else
                l = mid + 1;
        }
        if (!if_find)
            throw std::string("Error: Deleted a non-existent object.");
        int deleted_pos = object_node.data[pos].address;// 删除的内容在data_中的位置
        for (int i = pos; i < object_node.elements_num - 1; i++)
            object_node.data[i] = object_node.data[i + 1];
        object_node.elements_num--;
        record_num_--;
        if (pos == 0 && object_node.parent_num != -1)
            DeleteFix_(object_node.parent_num, key_in, object_node.data[0].key);
        // 实现外存回收
        data_memory_pool_.Add(deleted_pos);
        // 向外存进行记入
        index_.seekp(k_head_preserved + sizeof(Node_) * object_node.my_num);
        index_.write(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        // 检查是否需要并块
        if (object_node.elements_num <= k_min_size) {
            MergeNode_(object_node);
        }
        if (object_node == root_)
            root_ = object_node;
    }

    void InputInArray(key_type *key_array_in, value_type *value_array_in, int length) {
        for (int i = 0; i < length; i++) {
            Insert(key_array_in[i], value_array_in[i]);
        }
    }

    class Iterator {
    private:
        BPTree<key_type, value_type> *tree_;
        Node_ object_node_;
        int position_;

    public:
        explicit Iterator(BPTree<key_type, value_type> *tree_in) {
            tree_ = tree_in;
        }

        Iterator(const Iterator &rhs) {
            tree_ = rhs.tree_;
            object_node_ = rhs.object_node_;
            position_ = rhs.position_;
        }

        Iterator(BPTree<key_type, value_type> *tree_in, bool if_begin) {
            if (tree_in->record_num_ != 0) {
                if (if_begin) {
                    object_node_ = tree_in->GetFirstNode_();
                    position_ = 0;
                } else {
                    object_node_ = tree_in->GetLastNode_();
                    position_ = object_node_.elements_num;
                }
            } else
                throw std::string("Error: Empty BPlusTree.");
        }

        ~Iterator() {

        }

        bool operator==(const Iterator &rhs) const {
            return tree_ == rhs.tree_ && object_node_.data[position_].key == rhs.object_node_.data[rhs.position_].key;
        }

        bool operator!=(const Iterator &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const Iterator &rhs) const {
            if (tree_ != rhs.tree_)
                throw std::string("Error: Iterators of different BPTree.");
            return object_node_.data[position_].key < rhs.object_node_.data[rhs.position_].key;
        }

        bool operator>(const Iterator &rhs) const {
            if (tree_ != rhs.tree_)
                throw std::string("Error: Iterators of different BPTree.");
            return object_node_.data[position_].key > rhs.object_node_.data[rhs.position_].key;
        }

        bool operator<=(const Iterator &rhs) const {
            if (tree_ != rhs.tree_)
                throw std::string("Error: Iterators of different BPTree.");
            return object_node_.data[position_].key <= rhs.object_node_.data[rhs.position_].key;
        }

        bool operator>=(const Iterator &rhs) const {
            if (tree_ != rhs.tree_)
                throw std::string("Error: Iterators of different BPTree.");
            return object_node_.data[position_].key >= rhs.object_node_.data[rhs.position_].key;
        }

        const value_type &operator*() {
            // 无法引用
            if (position_ < 0 || position_ >= object_node_.elements_num)
                throw std::string("Error: Invalid Iterator operation.");
            value_type ans;
            tree_->data_.seekg(sizeof(value_type) * object_node_.data[position_].address);
            tree_->data_.read(reinterpret_cast<char *>(&ans), sizeof(value_type));
            return ans;
        }

        void SetPrev_() {
            if (position_ == 0) {
                if (object_node_.prev_num != -1) {
                    tree_->index_.seekg(k_head_preserved + sizeof(Node_) * object_node_.prev_num);
                    tree_->index_.read(reinterpret_cast<char *>(&object_node_), sizeof(Node_));
                    position_ = object_node_.elements_num - 1;
                } else
                    throw std::string("Error: Invalid Iterator operation.");
            } else {
                position_--;
            }
        }

        void SetNext_() {
            if (position_ == object_node_.elements_num - 1) {
                if (object_node_.next_num != -1) {
                    tree_->index_.seekg(k_head_preserved + sizeof(Node_) * object_node_.next_num);
                    tree_->index_.read(reinterpret_cast<char *>(&object_node_), sizeof(Node_));
                    position_ = 0;
                } else
                    position_++;
            } else if (position_ == object_node_.elements_num && object_node_ == tree_->GetLastNode_())
                throw std::string("Error: Invalid Iterator operation.");
            else {
                position_++;
            }
        }

        Iterator &operator++() {
            this->SetNext_();
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            this->SetNext_();
            return temp;
        }

        Iterator &operator--() {
            this->SetPrev_();
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            this->SetPrev_();
            return temp;
        }
    };

    Iterator Begin() {
        Iterator ans(this, true);
        return ans;
    }

    Iterator End() {
        Iterator ans(this, false);
        return ans;
    }

    void PrintInArray() {
        Iterator object = Begin();
        while (object != End())
            std::cout << *(object++) << ' ';
        std::cout << std::endl;
    }

private:
    Node_ FindObjectNode_(key_type key_in) {
        Node_ object_node;
        index_.seekg(k_head_preserved + sizeof(Node_) * root_.my_num);
        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        while (!object_node.if_leaf) {
            // 找到叶节点
            int l, r, mid;
            l = 0;
            r = object_node.elements_num - 2;
            while (l <= r) {
                mid = (l + r) / 2;
                if (object_node.data[mid].key > key_in)
                    r = mid - 1;
                else
                    l = mid + 1;
            }
            int pos = l;
            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[pos].address);
            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        }
        return object_node;
    }


    void NodeCopy_(Node_ &old_node_in, Node_ &new_node_in, int start_pos = 0, bool if_half = false) {
        // 将数据从旧块中拷到新块中
        if (start_pos >= old_node_in.elements_num || start_pos < 0)
            throw std::string("Error: Out of range.");
        if (if_half)
            start_pos = old_node_in.elements_num / 2;
        Node_ moved_node;
        if (old_node_in.if_leaf)
            for (int i = 0; i < old_node_in.elements_num - start_pos; i++)
                new_node_in.data[i] = old_node_in.data[i + start_pos];
        else
            for (int i = 0; i < old_node_in.elements_num - start_pos; i++) {
                new_node_in.data[i] = old_node_in.data[i + start_pos];
                index_.seekg(k_head_preserved + sizeof(Node_) * new_node_in.data[i].address);
                index_.read(reinterpret_cast<char *>(&moved_node), sizeof(Node_));
                moved_node.parent_num = new_node_in.my_num;
                index_.seekp(k_head_preserved + sizeof(Node_) * moved_node.my_num);
                index_.write(reinterpret_cast<char *>(&moved_node), sizeof(Node_));
            }
        new_node_in.elements_num = old_node_in.elements_num - start_pos;
        old_node_in.elements_num = start_pos;
    }

    void DataMerge_(Node_ &first_node_in, Node_ &second_node_in) {// 并块时还要改变parent的指向
        // 将两个块的数据合并到第一个块中
        if (!first_node_in.if_leaf) {
            Node_ son_node;
            for (int i = 0; i < second_node_in.elements_num; i++) {
                index_.seekg(k_head_preserved + sizeof(Node_) * second_node_in.data[i].address);
                index_.read(reinterpret_cast<char *>(&son_node), sizeof(Node_));
                son_node.parent_num = first_node_in.my_num;
                index_.seekp(k_head_preserved + sizeof(Node_) * second_node_in.data[i].address);
                index_.write(reinterpret_cast<char *>(&son_node), sizeof(Node_));
                first_node_in.data[first_node_in.elements_num + i] = second_node_in.data[i];
            }
        } else {
            for (int i = 0; i < second_node_in.elements_num; i++)
                first_node_in.data[first_node_in.elements_num + i] = second_node_in.data[i];
        }
//        if (!first_node_in.if_leaf) {
//            Node_ temp;
//            index_.seekg(k_head_preserved + sizeof(Node_) * second_node_in.data[0].address);
//            index_.read(reinterpret_cast<char *>(&temp), sizeof(Node_));
//            first_node_in.data[first_node_in.elements_num].key = temp.data[0].key;
//        }
        first_node_in.elements_num += second_node_in.elements_num;
    }

    void BreakNode_(Node_ node_in) {

//
//        std::cerr << "Broke node: " << node_in.my_num << ' ' << node_in.data[0].key << ' '
//                  << node_in.elements_num << std::endl;


        int new_node_num;
        if (node_memory_pool_.Empty()) {
            new_node_num = node_num_;
            node_num_++;
        } else
            new_node_num = node_memory_pool_.GetBack();
        Node_ new_node(new_node_num);
        new_node.if_leaf = node_in.if_leaf;
        if (node_in == root_) {
            // 是根节点时，需要增添一层节点
            Node_ new_root(node_num_);
            new_root.if_leaf = false;
            node_num_++;
            node_in.next_num = new_node.my_num;
            new_node.prev_num = node_in.my_num;
            node_in.parent_num = new_root.my_num;
            new_node.parent_num = new_root.my_num;
            NodeCopy_(node_in, new_node, 0, true);
            index_.seekp(k_head_preserved + sizeof(Node_) * node_in.my_num);
            index_.write(reinterpret_cast<char *>(&node_in), sizeof(Node_));
            index_.seekp(k_head_preserved + sizeof(Node_) * new_node.my_num);
            index_.write(reinterpret_cast<char *>(&new_node), sizeof(Node_));
            new_root.data[new_root.elements_num].key =
                    node_in.if_leaf ? new_node.data[0].key : node_in.data[node_in.elements_num - 1].key;
            new_root.data[new_root.elements_num].address = node_in.my_num;
            new_root.elements_num++;
            new_root.data[new_root.elements_num].address = new_node.my_num;
            new_root.elements_num++;
            root_ = new_root;
            root_num_ = root_.my_num;
            index_.seekp(k_head_preserved + sizeof(Node_) * new_root.my_num);
            index_.write(reinterpret_cast<char *>(&new_root), sizeof(Node_));
            // TODO: Debug 检查用
            if (new_root.elements_num > k_max_size) {
                node_memory_pool_.Add(new_node_num);
                throw std::string("Error: Programing bug: The maximum number is too small.Function: BreakNode_.");
            }
            height_++;
        } else {
            // 当不是根节点时，直接裂块即可
            if (node_in.next_num != -1) {
                // 当不是最后一块时
                Node_ next_node;
                index_.seekg(k_head_preserved + sizeof(Node_) * node_in.next_num);
                index_.read(reinterpret_cast<char *>(&next_node), sizeof(Node_));
                new_node.prev_num = node_in.my_num;
                new_node.next_num = next_node.my_num;
                node_in.next_num = new_node_num;
                next_node.prev_num = new_node_num;
                new_node.parent_num = node_in.parent_num;
                NodeCopy_(node_in, new_node, 0, true);
                index_.seekp(k_head_preserved + sizeof(Node_) * node_in.my_num);
                index_.write(reinterpret_cast<char *>(&node_in), sizeof(Node_));
                index_.seekp(k_head_preserved + sizeof(Node_) * next_node.my_num);
                index_.write(reinterpret_cast<char *>(&next_node), sizeof(Node_));
                index_.seekp(k_head_preserved + sizeof(Node_) * new_node.my_num);
                index_.write(reinterpret_cast<char *>(&new_node), sizeof(Node_));
            } else {
                // 是最后一块时
                node_in.next_num = new_node.my_num;
                new_node.prev_num = node_in.my_num;
                new_node.parent_num = node_in.parent_num;
                NodeCopy_(node_in, new_node, 0, true);
                index_.seekp(k_head_preserved + sizeof(Node_) * node_in.my_num);
                index_.write(reinterpret_cast<char *>(&node_in), sizeof(Node_));
                index_.seekp(k_head_preserved + sizeof(Node_) * new_node.my_num);
                index_.write(reinterpret_cast<char *>(&new_node), sizeof(Node_));
            }
            // 更改上一级的索引，此时必定能找到
            Node_ parent_node;
            index_.seekg(k_head_preserved + sizeof(Node_) * node_in.parent_num);
            index_.read(reinterpret_cast<char *>(&parent_node), sizeof(Node_));
            if (node_in.my_num == parent_node.data[parent_node.elements_num - 1].address) {// 裂的是最尾端的块
                parent_node.data[parent_node.elements_num].address = new_node.my_num;
                parent_node.data[parent_node.elements_num - 1].key =
                        node_in.if_leaf ? new_node.data[0].key : node_in.data[node_in.elements_num - 1].key;
                parent_node.elements_num++;
            } else {
                int l, r, mid, pos;
                l = 0;
                r = parent_node.elements_num - 2;
                while (l <= r) {
                    mid = (l + r) / 2;
                    if (parent_node.data[mid].key > new_node.data[new_node.elements_num - 1].key)
                        r = mid - 1;
                    else
                        l = mid + 1;
                }
                pos = l;
                // TODO: Debug 检查用
                if (parent_node.data[pos].address != node_in.my_num)
                    throw std::string("Error: Programing bug. Function: BreakNode_.");

                parent_node.data[pos].address = new_node.my_num;
                for (int i = parent_node.elements_num; i > pos + 1; i--)
                    parent_node.data[i] = parent_node.data[i - 1];
                parent_node.data[pos + 1].key = parent_node.data[pos].key;
                parent_node.data[pos + 1].address = new_node.my_num;
                parent_node.data[pos].key =
                        node_in.if_leaf ? new_node.data[0].key : node_in.data[node_in.elements_num - 1].key;
                parent_node.elements_num++;
            }
            index_.seekp(k_head_preserved + sizeof(Node_) * parent_node.my_num);
            index_.write(reinterpret_cast<char *>(&parent_node), sizeof(Node_));
            if (parent_node.my_num == root_.my_num)
                root_ = parent_node;
            // 若上一级块数超了，则需要进一步对上一级进行裂块
            if (parent_node.elements_num >= k_max_size)
                BreakNode_(parent_node);
        }
    }

    void DeleteFix_(int parent_node_num, key_type deleted_key, key_type replaced_key) {
        // 如果删除了块的第一个元素，则其parent的节点也要相应的变化
        Node_ parent_node;
        index_.seekg(k_head_preserved + sizeof(Node_) * parent_node_num);
        index_.read(reinterpret_cast<char *>(&parent_node), sizeof(Node_));
        int l, r, mid, pos;
        pos = -1;
        l = 0;
        r = parent_node.elements_num - 2;
        bool if_find = false;
        while (l <= r) {
            mid = (l + r) / 2;
            if (parent_node.data[mid].key == deleted_key) {
                pos = mid;
                if_find = true;
                break;
            } else if (parent_node.data[mid].key > deleted_key)
                r = mid - 1;
            else
                l = mid + 1;
        }
        if (!if_find) {// 说明删掉的是 parent.data[0] 的第一个,应在上一级进行更改
            if (parent_node.parent_num != -1)
                DeleteFix_(parent_node.parent_num, deleted_key, replaced_key);
        } else {
            parent_node.data[pos].key = replaced_key;
            index_.seekp(k_head_preserved + sizeof(Node_) * parent_node_num);
            index_.write(reinterpret_cast<char *>(&parent_node), sizeof(Node_));
        }
//        if (pos == -1)
//            pos = l;
//        parent_node.data[pos].key = object_node.data[0].key;
//        if (pos == 0 && parent_node.parent_num != -1)
//            DeleteFix_(parent_node, deleted_key);
    }

    void MergeNode_(Node_ &node_in) {

//        std::cerr << "Merged node: " << node_in.my_num << ' ' << node_in.data[0].key << ' '
//                  << node_in.elements_num << std::endl;


        Node_ first_node;
        Node_ second_node;
        Node_ parent_node;
        if (node_in.parent_num == -1) {
            // 若是最顶上的一块，应检查是否需要减层数
            if (node_in.prev_num != -1 || node_in.next_num != -1)
                throw std::string("Error: Programing bug. Function: MergeNode_.");
            if (node_in.elements_num <= 1 && height_ > 1) {
                index_.seekg(k_head_preserved + sizeof(Node_) * node_in.data[0].address);
                index_.read(reinterpret_cast<char *>(&root_), sizeof(Node_));
                root_.parent_num = -1;
                root_num_ = root_.my_num;
                index_.seekp(k_head_preserved + sizeof(Node_) * root_.my_num);
                index_.write(reinterpret_cast<char *>(&root_), sizeof(Node_));
                //外存回收
                node_memory_pool_.Add(node_in.my_num);
                height_--;
                if (root_.elements_num >= k_max_size)
                    BreakNode_(root_);


//                std::cerr << "Height decreased: " << node_in.elements_num << std::endl;
            }
            return;
        }
        index_.seekg(k_head_preserved + sizeof(Node_) * node_in.parent_num);
        index_.read(reinterpret_cast<char *>(&parent_node), sizeof(Node_));
        if (node_in.data[0].key < parent_node.data[0].key) {// 这个判断的是是否为当前节点的父节点的第一块
            // 若是第一块
            first_node = node_in;
            index_.seekg(k_head_preserved + sizeof(Node_) * node_in.next_num);
            index_.read(reinterpret_cast<char *>(&second_node), sizeof(Node_));
        } else {
            second_node = node_in;
            index_.seekg(k_head_preserved + sizeof(Node_) * node_in.prev_num);
            index_.read(reinterpret_cast<char *>(&first_node), sizeof(Node_));
            node_in = first_node;
        }
        DataMerge_(first_node, second_node);
        first_node.next_num = second_node.next_num;
        if (second_node.next_num != -1) {
            Node_ next_node;
            index_.seekg(k_head_preserved + sizeof(Node_) * second_node.next_num);
            index_.read(reinterpret_cast<char *>(&next_node), sizeof(Node_));
            next_node.prev_num = first_node.my_num;
            index_.seekp(k_head_preserved + sizeof(Node_) * next_node.my_num);
            index_.write(reinterpret_cast<char *>(&next_node), sizeof(Node_));
        }
        //外存回收
        node_memory_pool_.Add(second_node.my_num);
        // 更改父节点的索引，此时必定能够找到
        int l, r, mid, pos;
        l = 0;
        r = parent_node.elements_num - 2;
        while (l <= r) {
            mid = (l + r) / 2;
            if (parent_node.data[mid].key > second_node.data[0].key)
                r = mid - 1;
            else
                l = mid + 1;
        }
        pos = l - 1;
        // 若为内节点，还需更改 first_node 原最后一个的key值
        if (!first_node.if_leaf)
            first_node.data[first_node.elements_num - second_node.elements_num - 1].key = parent_node.data[pos].key;
        index_.seekp(k_head_preserved + sizeof(Node_) * first_node.my_num);
        index_.write(reinterpret_cast<char *>(&first_node), sizeof(Node_));
        // 更改父节点的索引值
        for (int i = pos; i < parent_node.elements_num - 1; i++)
            parent_node.data[i].key = parent_node.data[i + 1].key;
        for (int i = pos + 1; i < parent_node.elements_num - 1; i++)
            parent_node.data[i].address = parent_node.data[i + 1].address;
        parent_node.elements_num--;
        index_.seekp(k_head_preserved + sizeof(Node_) * parent_node.my_num);
        index_.write(reinterpret_cast<char *>(&parent_node), sizeof(Node_));

        // 若上一级块过小，则也需进行并块
        if (parent_node.elements_num <= k_min_size) {
            MergeNode_(parent_node);
            index_.seekg(k_head_preserved + sizeof(Node_) * first_node.my_num);
            index_.read(reinterpret_cast<char *>(&first_node), sizeof(Node_));
        }
        // 对并后的块进行检查，看是否需要裂块
        if (first_node.elements_num >= k_max_size)
            BreakNode_(first_node);
    }

    Node_ GetFirstNode_() {
        Node_ object_node;
        index_.seekg(k_head_preserved);
        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        while (!object_node.if_leaf) {
            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[0].address);
            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        }
        return object_node;
    }

    Node_ GetLastNode_() {
        Node_ object_node;
        index_.seekg(k_head_preserved);
        index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        while (!object_node.if_leaf) {
            index_.seekg(k_head_preserved + sizeof(Node_) * object_node.data[object_node.elements_num - 1].address);
            index_.read(reinterpret_cast<char *>(&object_node), sizeof(Node_));
        }
        return object_node;
    }

};

#endif //MYBPLUSTREE_BPLUSTREE_H
