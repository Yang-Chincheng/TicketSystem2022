//
// Created by fir on 2022/6/9.
// this is a backup class written for the Ticket System.
//

#ifndef MYBPLUSTREE_BACKUP_H
#define MYBPLUSTREE_BACKUP_H

#include <fstream>
#include <iostream>

const int k_head_preserved_backup = sizeof(int) * 5;

template<class key_type, class value_type>
class Backup {
private:
    struct Record {
        int time;
        int op;// 0-change,1-insert,2-delete
        key_type key;
        value_type data; // data before the operation

        Record(int time_in, int op_in, const key_type &key_in, const value_type &value_in) {
            time = time_in;
            op = op_in;
            key = key_in;
            data = value_in;
        }
        Record(){};
    };

//    int time_now_;
    int record_num_;
    std::fstream file_;
    std::string file_name_;

public:
    explicit Backup(const std::string &name_in) {
        file_name_ = name_in + std::string("_backup_storage");
        file_.open("./bin/" + file_name_);
        if (!file_) {
            file_.open("./bin/" + file_name_, std::ostream::out);
//            time_now_ = 0;
            record_num_ = 0;
            file_.seekp(0);
            file_.write(reinterpret_cast<char *>(&record_num_), sizeof(int));
//            file_.write(reinterpret_cast<char *>(&time_now_), sizeof(int));
            file_.close();
            file_.open("./bin/" + file_name_);
        } else {
            file_.seekg(0);
            file_.read(reinterpret_cast<char *>(&record_num_), sizeof(int));
//            file_.read(reinterpret_cast<char *>(&time_now_), sizeof(int));
        }
    }

    ~Backup() {
        file_.seekp(0);
        file_.write(reinterpret_cast<char *>(&record_num_), sizeof(int));
//        file_.write(reinterpret_cast<char *>(&time_now_), sizeof(int));
    }

    bool Empty() {
        return record_num_ == 0;
    }

    int Size() {
        return record_num_;
    }

    int Time() {
        Record record;
        file_.seekg(k_head_preserved_backup + sizeof(Record) * (record_num_ - 1));
        file_.read(reinterpret_cast<char *>(&record), sizeof(Record));
        return record.time;
    }

    void AddRecord(int time, int op, const key_type key_in, const value_type &value_before) {
        Record record(time, op, key_in, value_before);
        file_.seekp(k_head_preserved_backup + sizeof(Record) * record_num_);
        file_.write(reinterpret_cast<char *>(&record), sizeof(record));
        record_num_++;
//        time_now_=time;
    }

    void LastRecord(int &time, int &op, key_type &key, value_type &value) {
        Record record;
        file_.seekg(k_head_preserved_backup + sizeof(Record) *(record_num_-1));
        file_.read(reinterpret_cast<char *>(&record), sizeof(Record));
        time = record.time;
        op = record.op;
        key = record.key;
        value = record.data;
    }

    void Pop(){
        record_num_--;
    }
};

#endif //MYBPLUSTREE_BACKUP_H
