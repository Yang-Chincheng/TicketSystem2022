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
    char spilt_;
    unsigned long length_;
    unsigned long pos_;
public:
    TokenScanner(char spilt_in = ' ') {}

    ~TokenScanner() {}

    TokenScanner(const std::string &buff_in, char spilt_in = ' ') : buff_(buff_in), spilt_(spilt_in) {
        length_ = buff_.length();
        pos_ = 0;
    }

    void Set_Spilt(char spilt_in){
        pos_=0;
        spilt_=spilt_in;
    }

    template<class T>
    T Next_Token() {
        std::string ans;
        while (buff_[pos_] == spilt_ && pos_ < length_)
            pos_++;
        while (buff_[pos_] != spilt_ && pos_ < length_) {
            ans += buff_[pos_];
            pos_++;
        }
        return T(ans);
    }

    void RollBack() { //重新处理此命令
        pos_ = 0;
    }

    bool End_() {
        bool is_end = false;
        while (buff_[pos_] == spilt_) {
            pos_++;
            if (pos_ == length_) {
                is_end = true;
                break;
            }
        }
        return is_end;
    }
};
template<>
int TokenScanner::Next_Token<int>() {
    int ans = 0;
    while (buff_[pos_] == spilt_ && pos_ < length_)
        pos_++;
    while (buff_[pos_] != spilt_ && pos_ < length_) {
        ans = ans * 10 + int(buff_[pos_]);
        pos_++;
    }
    return ans;
}
#endif //MYBPLUSTREE_TOKENSCANNER_H