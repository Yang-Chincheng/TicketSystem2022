#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../lib/utility.h"
#include "ticketsystem.h"
#include <string>

inline ticket::Date getDate(const std::string &str) {
    TokenScanner scan(str, '-');
    return ticket::Date(scan.Next_Token<int>(), scan.Next_Token<int>());
}

inline ticket::Time getTime(const std::string &str) {
    TokenScanner scan(str, ':');
    return ticket::Time(scan.Next_Token<int>(), scan.Next_Token<int>());
}

int main() {
    ticket::SysManager ticksys;
    std::string cmd;
    while(1) {
        getline(std::cin, cmd);
        TokenScanner scan(cmd);
        
        std::string opt_idx = scan.Next_Token();
        std::string opt = scan.Next_Token();

        try {
            if(opt == "add_user") {
                assert(scan.Next_Token() == "-c");
                ticket::Username cur_user = scan.Next_Token<ticket::Username>();
                assert(scan.Next_Token() == "-u");
                ticket::Username new_user = scan.Next_Token<ticket::Username>();
                assert(scan.Next_Token() == "-p");
                ticket::Password pwd = scan.Next_Token<ticket::Password>();
                assert(scan.Next_Token() == "-n");
                ticket::Name name = scan.Next_Token<ticket::Name>();
                assert(scan.Next_Token() == "-m");
                ticket::MailAddr maddr = scan.Next_Token<ticket::MailAddr>();
                assert(scan.Next_Token() == "-g");
                int priv = scan.Next_Token<int>();
                assert(scan.Is_End());
                ticksys.add_user(
                    opt_idx, cur_user, new_user, pwd, name, maddr, priv 
                );
            }
            else if(opt == "login") {
                assert(scan.Next_Token() == "-u");
                ticket::Username user = scan.Next_Token<ticket::Username>();
                assert(scan.Next_Token() == "-p");
                ticket::Password pwd = scan.Next_Token<ticket::Password>();
                assert(scan.Is_End());
                ticksys.login(opt_idx, user, pwd);
            }
            else if(opt == "logout") {
                assert(scan.Next_Token() == "-u");
                ticket::Username user = scan.Next_Token<ticket::Username>();
                assert(scan.Is_End());
                ticksys.logout(opt_idx, user);
            }
            else if(opt == "query_profile") {
                assert(scan.Next_Token() == "-c");
                ticket::Username cur_user = scan.Next_Token<ticket::Username>();
                assert(scan.Next_Token() == "-u");
                ticket::Username qry_user = scan.Next_Token<ticket::Username>();
                assert(scan.Is_End());
                ticksys.query_profile(opt_idx, cur_user, qry_user);
            }
            else if(opt == "modify_profile") {
                assert(scan.Next_Token() == "-c");
                ticket::Username cur_user = scan.Next_Token<ticket::Username>();
                assert(scan.Next_Token() == "-u");
                ticket::Username mod_user = scan.Next_Token<ticket::Username>();
                ticket::Password pwd; // defualt: ""
                ticket::Name name; // defualt: ""
                ticket::MailAddr maddr; // default: ""
                int priv = -1; // default: -1
                while(!scan.Is_End()) {
                    std::string arg = scan.Next_Token();
                    if(arg == "-p") {
                        pwd = scan.Next_Token<ticket::Password>();
                    }
                    else if(arg == "-n") {
                        name = scan.Next_Token<ticket::Name>();
                    }
                    else if(arg == "-m") {
                        maddr = scan.Next_Token<ticket::MailAddr>();
                    }
                    else if(arg == "-g") {
                        priv = scan.Next_Token<int>();
                    }
                    else assert(0);
                }
                ticksys.modify_profile(
                    opt_idx, cur_user, mod_user, pwd, name, maddr, priv
                );
            }

            else if(opt == "add_train") {
                assert(scan.Next_Token() == "-i");
                ticket::TrainID id = scan.Next_Token<ticket::TrainID>();
                assert(scan.Next_Token() == "-n");
                int sta_num = scan.Next_Token<int>();
                assert(scan.Next_Token() == "-m");
                int seat_num = scan.Next_Token<int>();
                assert(scan.Next_Token() == "-s");
                TokenScanner sta_scan(scan.Next_Token(), '|');
                ticket::Station sta[110]; // 1:n
                for(int i = 1; i <= sta_num; ++i) {
                    assert(!sta_scan.Is_End());
                    sta[i] = sta_scan.Next_Token<ticket::Station>();
                }
                assert(sta_scan.Is_End());
                assert(scan.Next_Token() == "-p");
                TokenScanner pri_scan(scan.Next_Token(), '|');
                int price[110]; // 2:n
                for(int i = 2; i <= sta_num; ++i) {
                    assert(!pri_scan.Is_End());
                    price[i] = pri_scan.Next_Token<int>();
                }
                assert(pri_scan.Is_End());
                assert(scan.Next_Token() == "-x");
                ticket::Time st_time = getTime(scan.Next_Token());
                assert(scan.Next_Token() == "-t");
                TokenScanner tr_time_scan(scan.Next_Token(), '|');
                int tra[110]; // 1:n-1
                for(int i = 1; i < sta_num; ++i) {
                    assert(!tr_time_scan.Is_End());
                    tra[i] = tr_time_scan.Next_Token<int>();
                }
                assert(tr_time_scan.Is_End());
                assert(scan.Next_Token() == "-o");
                TokenScanner sp_time_scan(scan.Next_Token(), '|');
                int stp[110]; // 2:n-1
                for(int i = 2; i < sta_num; ++i) {
                    assert(!sp_time_scan.Is_End());
                    stp[i] = sp_time_scan.Next_Token<int>();
                }
                assert(sp_time_scan.Is_End());
                assert(scan.Next_Token() == "-d");
                TokenScanner date_scan(scan.Next_Token(), '|');
                ticket::Date st_date = getDate(date_scan.Next_Token());
                ticket::Date ed_date = getDate(date_scan.Next_Token());
                assert(date_scan.Is_End());
                assert(scan.Next_Token() == "-y");
                char type = (scan.Next_Token())[0];
                assert(scan.Is_End());
                ticksys.add_train(
                    opt_idx, id, sta_num, seat_num, sta, price,
                    st_time, tra, stp, st_date, ed_date, type
                );
            }
            else if(opt == "delete_train") {
                assert(scan.Next_Token() == "-i");
                ticket::TrainID id = scan.Next_Token<ticket::TrainID>();
                assert(scan.Is_End());
                ticksys.delete_train(opt_idx, id);
            }
            else if(opt == "release_train") {
                assert(scan.Next_Token() == "-i");
                ticket::TrainID id = scan.Next_Token<ticket::TrainID>();
                assert(scan.Is_End());
                ticksys.release_train(opt_idx, id);
            }
            else if(opt == "query_train") {
                assert(scan.Next_Token() == "-i");
                ticket::TrainID id = scan.Next_Token<ticket::TrainID>();
                assert(scan.Next_Token() == "-d");
                ticket::Date date = getDate(scan.Next_Token());
                assert(scan.Is_End());
                ticksys.query_train(opt_idx, id, date);
            }
            else if(opt == "query_ticket") {
                assert(scan.Next_Token() == "-s");
                ticket::Station st = scan.Next_Token<ticket::Station>();
                assert(scan.Next_Token() == "-t");
                ticket::Station tr = scan.Next_Token<ticket::Station>();
                assert(scan.Next_Token() == "-d");
                ticket::Date date = getDate(scan.Next_Token());
                bool cmp_type = 0;
                if(!scan.Is_End()) {
                    assert(scan.Next_Token() == "-p");
                    cmp_type = scan.Next_Token() == "cost";
                }
                ticksys.query_ticket(opt_idx, date, st, tr, cmp_type);
            }
            else if(opt == "query_transfer") {
                assert(scan.Next_Token() == "-s");
                ticket::Station st = scan.Next_Token<ticket::Station>();
                assert(scan.Next_Token() == "-t");
                ticket::Station tr = scan.Next_Token<ticket::Station>();
                assert(scan.Next_Token() == "-d");
                ticket::Date date = getDate(scan.Next_Token());
                bool cmp_type = 0;
                if(!scan.Is_End()) {
                    assert(scan.Next_Token() == "-p");
                    cmp_type = scan.Next_Token() == "cost";
                }
                ticksys.query_transfer(opt_idx, date, st, tr, cmp_type);
            }

            else if(opt == "buy_ticket") {
                assert(scan.Next_Token() == "-u");
                ticket::Username user = scan.Next_Token<ticket::Username>();
                assert(scan.Next_Token() == "-i");
                ticket::TrainID id = scan.Next_Token<ticket::TrainID>();
                assert(scan.Next_Token() == "-d");
                ticket::Date date = getDate(scan.Next_Token());
                assert(scan.Next_Token() == "-n");
                int num = scan.Next_Token<int>();
                assert(scan.Next_Token() == "-f");
                ticket::Station st = scan.Next_Token<ticket::Station>();
                assert(scan.Next_Token() == "-t");
                ticket::Station tr = scan.Next_Token<ticket::Station>();
                bool trax_type = 0;
                if(!scan.Is_End()) {
                    assert(scan.Next_Token() == "-q");
                    trax_type = scan.Next_Token() == "true";
                }
                ticksys.buy_ticket(opt_idx, user, id, date, st, tr, num, trax_type);
            }
            else if(opt == "query_order") {
                assert(scan.Next_Token() == "-u");
                ticket::Username user = scan.Next_Token<ticket::Username>();
                assert(scan.Is_End());
                ticksys.query_order(opt_idx, user);
            }
            else if(opt == "refund_ticket") {
                assert(scan.Next_Token() == "-u");
                ticket::Username user = scan.Next_Token<ticket::Username>();
                int idx = 0;
                if(!scan.Is_End()) {
                    assert(scan.Next_Token() == "-n");
                    idx = scan.Next_Token<int>() - 1;
                }
                ticksys.refund_ticket(opt_idx, user, idx);
            }

            else if(opt == "clean") {
                ticksys.clean(opt_idx);   
            }
            else if(opt == "exit") {
                ticksys.exit(opt_idx);
            }  
            else assert(0);

        }
        catch(ticket::exception e) {
            std::cout << opt_idx << " -1" << std::endl;
        }
    }
    return 0;
}