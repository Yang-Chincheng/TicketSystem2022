//
// Created by fir on 2022/5/17.
//

#ifndef MYBPLUSTREE_TOKENSCANNER_H
#define MYBPLUSTREE_TOKENSCANNER_H

#include <iostream>
#include <cstring>

class TokenScanner {
private:
    std::string buff_;
    unsigned long length_;
    unsigned long pos_;
public:
    TokenScanner() {}

    ~TokenScanner() {}

    TokenScanner(const std::string &buff_in) : buff_(buff_in) {
        length_ = buff_.length();
        pos_ = 0;
    }

    template<class T>
    T Next_Token() {
        std::string ans;
        while (buff_[pos_] == ' ' && pos_ < length_)
            pos_++;
        while (buff_[pos_] != ' ' && pos_ < length_) {
            ans += buff_[pos_];
            pos_++;
        }
        return T(ans);
    }

    void RollBack() { //重新处理此条指令
        pos_ = 0;
    }

    bool End_() {
        bool is_end = false;
        while (buff_[pos_] == ' ') {
            pos_++;
            if (pos_ == length_) {
                is_end = true;
                break;
            }
        }
        return is_end;
    }
};

#endif //MYBPLUSTREE_TOKENSCANNER_H
