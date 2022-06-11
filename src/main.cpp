// #define TICKSYS_DEBUG
#define TICKSYS_ROLLBACK
// #define TIME_INSPECT

#include "../lib/utility.h"
#include "ticketsystem.h"
#include <string>
#ifdef TIME_INSPECT
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <time.h>
#endif
#ifdef RESOURCE_INSPECT
    #include <sys/resource.h>
#endif

inline ticket::Date getDate(const std::string &str) {
    TokenScanner scan(str, '-');
    int mm = scan.Next_Token<int>();
    int dd = scan.Next_Token<int>();
    return ticket::Date(mm, dd);
}

inline ticket::Time getTime(const std::string &str) {
    TokenScanner scan(str, ':');
    int hh = scan.Next_Token<int>();
    int mm = scan.Next_Token<int>();
    return ticket::Time(hh, mm);
}

#ifdef RESOURCE_INSPECT
void get_exe_resource_limits() {
    rlimit my_rlimit;
    getrlimit(RLIMIT_AS, &my_rlimit);
    std::cerr << my_rlimit.rlim_cur << " " << my_rlimit.rlim_max << std::endl;
    getrlimit(RLIMIT_DATA, &my_rlimit);
    std::cerr << my_rlimit.rlim_cur << " " << my_rlimit.rlim_max << std::endl;
    getrlimit(RLIMIT_FSIZE, &my_rlimit);
    std::cerr << my_rlimit.rlim_cur << " " << my_rlimit.rlim_max << std::endl;
    getrlimit(RLIMIT_STACK, &my_rlimit);
    std::cerr << my_rlimit.rlim_cur << " " << my_rlimit.rlim_max << std::endl;
}
#endif

#ifdef TIME_INSPECT
struct timer {
    int sysHz;
    unsigned long mark;
    unsigned long tick; 

    timer() {
        sysHz = sysconf(_SC_CLK_TCK);
    }

    void start() {
        mark = time(NULL);
    }

    void stop() {
        tick += time(NULL) - mark;
    }

    double total() {
        return 1.0 * tick / sysHz;
    }

} quser, qtick, btick, qtran, qorder, rtick;
#endif

int main() {
    std::cout.sync_with_stdio(false);
#ifdef RESOURCE_INSPECT
    get_exe_resource_limits();
#endif
    ticket::SysManager ticksys;
    std::string cmd;
    while(1) {
        getline(std::cin, cmd);
        TokenScanner scan(cmd);
        
        std::string opt_idx_str = scan.Next_Token();
        int opt_idx = TokenScanner(opt_idx_str.substr(1), ']').Next_Token<int>();
        std::string opt = scan.Next_Token();
        std::string tag;

// std::cout << opt_idx_str << " " << opt << std::endl;

        try {
            if(opt == "add_user") {
#ifdef TIME_INSPECT
quser.start();
#endif
                ticket::Username cur_user;
                ticket::Username new_user;
                ticket::Password pwd;
                ticket::Name name;
                ticket::MailAddr maddr;
                int priv;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-c")
                        cur_user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-u")
                        new_user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-p")                    
                        pwd = scan.Next_Token<ticket::Password>();
                    else if(tag == "-n")                    
                        name = scan.Next_Token<ticket::Name>();
                    else if(tag == "-m")                    
                        maddr = scan.Next_Token<ticket::MailAddr>();
                    else if(tag == "-g")                    
                        priv = scan.Next_Token<int>();
                    else ASSERT(0);
                }
                ticksys.add_user(
                    opt_idx, cur_user, new_user, pwd, name, maddr, priv 
                );
#ifdef TIME_INSPECT
quser.stop();
#endif
            }
            else if(opt == "login") {
#ifdef TIME_INSPECT
quser.start();
#endif
                ticket::Username user;
                ticket::Password pwd;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-u")
                        user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-p")
                        pwd = scan.Next_Token<ticket::Password>();
                    else ASSERT(0);
                }
                ticksys.login(opt_idx, user, pwd);
#ifdef TIME_INSPECT
quser.stop();
#endif
            }
            else if(opt == "logout") {
#ifdef TIME_INSPECT
quser.start();
#endif
                ticket::Username user;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-u")
                        user = scan.Next_Token<ticket::Username>();
                    else ASSERT(0);
                }
                ticksys.logout(opt_idx, user);
#ifdef TIME_INSPECT
quser.stop();
#endif
            }
            else if(opt == "query_profile") {
#ifdef TIME_INSPECT
quser.start();
#endif
                ticket::Username cur_user;
                ticket::Username qry_user;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-c")
                        cur_user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-u")
                        qry_user = scan.Next_Token<ticket::Username>();
                    else ASSERT(0);
                }
                ticksys.query_profile(opt_idx, cur_user, qry_user);
#ifdef TIME_INSPECT
quser.stop();
#endif
            }
            else if(opt == "modify_profile") {
#ifdef TIME_INSPECT
quser.start();
#endif
                ticket::Username cur_user;
                ticket::Username mod_user;
                ticket::Password pwd; // defualt: ""
                ticket::Name name; // defualt: ""
                ticket::MailAddr maddr; // default: ""
                int priv = -1; // default: -1
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-c")
                        cur_user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-u")
                        mod_user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-p")
                        pwd = scan.Next_Token<ticket::Password>();
                    else if(tag == "-n")
                        name = scan.Next_Token<ticket::Name>();
                    else if(tag == "-m")
                        maddr = scan.Next_Token<ticket::MailAddr>();
                    else if(tag == "-g")
                        priv = scan.Next_Token<int>();
                    else ASSERT(0);
                }
                ticksys.modify_profile(
                    opt_idx, cur_user, mod_user, pwd, name, maddr, priv
                );
#ifdef TIME_INSPECT
quser.stop();
#endif
            }

            else if(opt == "add_train") {
                ticket::TrainID id;
                int sta_num;
                int seat_num;
                ticket::Station sta[110]; // 1:n
                int price[110]; // 2:n
                int tra[110]; // 1:n-1
                int stp[110]; // 2:n-1
                ticket::Time st_time;
                ticket::Date st_date;
                ticket::Date ed_date;
                char type;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-i") 
                        id = scan.Next_Token<ticket::TrainID>();
                    else if(tag == "-n") 
                        sta_num = scan.Next_Token<int>();
                    else if(tag == "-m")
                        seat_num = scan.Next_Token<int>();
                    else if(tag == "-s") {
                        TokenScanner sta_scan(scan.Next_Token(), '|');
                        int i = 1;
                        while(!sta_scan.Is_End()) {
                            sta[i++] = sta_scan.Next_Token<ticket::Station>();
                        }
                    }
                    else if(tag == "-p") {
                        TokenScanner pri_scan(scan.Next_Token(), '|');
                        int i = 2;
                        while(!pri_scan.Is_End())
                            price[i++] = pri_scan.Next_Token<int>();
                    }
                    else if(tag == "-x") 
                        st_time = getTime(scan.Next_Token());
                    else if(tag == "-t") {
                        TokenScanner tr_time_scan(scan.Next_Token(), '|');
                        int i = 1;
                        while(!tr_time_scan.Is_End())
                            tra[i++] = tr_time_scan.Next_Token<int>();
                    }
                    else if(tag == "-o") {
                        TokenScanner sp_time_scan(scan.Next_Token(), '|');
                        int i = 2;
                        while(!sp_time_scan.Is_End())
                            stp[i++] = sp_time_scan.Next_Token<int>();
                    }
                    else if(tag == "-d") {
                        TokenScanner date_scan(scan.Next_Token(), '|');
                        st_date = getDate(date_scan.Next_Token());
                        ed_date = getDate(date_scan.Next_Token());
                    }
                    else if(tag == "-y")
                        type = (scan.Next_Token())[0];
                    else ASSERT(0);
                }
                ticksys.add_train(
                    opt_idx, id, sta_num, seat_num, sta, price,
                    st_time, tra, stp, st_date, ed_date, type
                );
            }
            else if(opt == "delete_train") {
                ticket::TrainID id;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-i")
                        id = scan.Next_Token<ticket::TrainID>();
                    else ASSERT(0);
                }
                ticksys.delete_train(opt_idx, id);
            }
            else if(opt == "release_train") {
                ticket::TrainID id;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-i")
                        id = scan.Next_Token<ticket::TrainID>();
                    else ASSERT(0);
                }
                ticksys.release_train(opt_idx, id);
            }
            else if(opt == "query_train") {
                ticket::TrainID id;
                ticket::Date date;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-i")
                        id = scan.Next_Token<ticket::TrainID>();
                    else if(tag == "-d")
                        date = getDate(scan.Next_Token());
                    else ASSERT(0);
                }
                ticksys.query_train(opt_idx, id, date);
            }
            else if(opt == "query_ticket") {
#ifdef TIME_INSPECT
qtick.start();
#endif
                ticket::Station st;
                ticket::Station tr;
                ticket::Date date;
                bool cmp_type = 0;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-s")
                        st = scan.Next_Token<ticket::Station>();
                    else if(tag == "-t")
                        tr = scan.Next_Token<ticket::Station>();
                    else if(tag == "-d")
                        date = getDate(scan.Next_Token());
                    else if(tag == "-p")
                        cmp_type = scan.Next_Token() == "cost";
                    else ASSERT(0);
                }
                ticksys.query_ticket(opt_idx, st, tr, date, cmp_type);
#ifdef TIME_INSPECT
qtick.stop();
#endif
            }
            else if(opt == "query_transfer") {
#ifdef TIME_INSPECT
qtran.start();
#endif
                ticket::Station st;
                ticket::Station tr;
                ticket::Date date;
                bool cmp_type = 0;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-s")
                        st = scan.Next_Token<ticket::Station>();
                    else if(tag == "-t")
                        tr = scan.Next_Token<ticket::Station>();
                    else if(tag == "-d")
                        date = getDate(scan.Next_Token());
                    else if(tag == "-p")
                        cmp_type = scan.Next_Token() == "cost";
                    else ASSERT(0);
                }
                ticksys.query_transfer(opt_idx, st, tr, date, cmp_type);
#ifdef TIME_INSPECT
qtran.stop();
#endif
            }

            else if(opt == "buy_ticket") {
#ifdef TIME_INSPECT
btick.start();
#endif
                ticket::Username user;
                ticket::TrainID id;
                ticket::Date date;
                int num;
                bool trax_type = 0;
                ticket::Station st;
                ticket::Station tr;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-u")
                        user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-i")
                        id = scan.Next_Token<ticket::TrainID>();
                    else if(tag == "-d")
                        date = getDate(scan.Next_Token());
                    else if(tag == "-n")
                        num = scan.Next_Token<int>();
                    else if(tag == "-f")
                        st = scan.Next_Token<ticket::Station>();
                    else if(tag == "-t")
                        tr = scan.Next_Token<ticket::Station>();
                    else if(tag == "-q")
                        trax_type = scan.Next_Token() == "true";
                    else ASSERT(0);
                }
                ticksys.buy_ticket(opt_idx, user, id, date, st, tr, num, trax_type);
#ifdef TIME_INSPECT
btick.stop();
#endif
            }
            else if(opt == "query_order") {
#ifdef TIME_INSPECT
qorder.start();
#endif
                ticket::Username user;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-u")
                        user = scan.Next_Token<ticket::Username>();
                    else ASSERT(0);
                }
                ticksys.query_order(opt_idx, user);
#ifdef TIME_INSPECT
qorder.stop();
#endif
            }
            else if(opt == "refund_ticket") {
#ifdef TIME_INSPECT
rtick.start();
#endif
                ticket::Username user;
                int idx = 0;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-u")
                        user = scan.Next_Token<ticket::Username>();
                    else if(tag == "-n")
                        idx = scan.Next_Token<int>();
                    else ASSERT(0);
                }
                ticksys.refund_ticket(opt_idx, user, idx);
#ifdef TIME_INSPECT
rtick.stop();
#endif
            }

            else if(opt == "rollback") {
                // assert(0);
                int time_idx;
                while(!scan.Is_End()) {
                    tag = scan.Next_Token();
                    if(tag == "-t")
                        time_idx = scan.Next_Token<int>();
                    else ASSERT(0);
                }
                ticksys.rollback(opt_idx, time_idx);   
            }

            else if(opt == "clean") {
                ticksys.clean(opt_idx);   
            }
            else if(opt == "exit") {
                ticksys.exit(opt_idx);
                break;
            }  
            else {
                std::cerr << opt_idx_str << " error: " << opt << std::endl;
                ASSERT(0); 
            }

        }
        catch(ticket::exception &e) {
            std::cout << opt_idx_str << " -1" << std::endl;
            if(opt_idx_str == "[4463]")
                std::cerr << ">> " << opt_idx << " note: " << e.what() << std::endl;
        }
        catch(std::string &msg) {
            std::cerr << "[error] " << msg << std::endl;
        }
        catch(...) {
            std::cerr << "[error] catch something else." << std::endl;
        }

    }

#ifdef TIME_INSPECT
std::cerr << "quser: " << quser.total() << std::endl;
std::cerr << "qtick: " << qtick.total() << std::endl;
std::cerr << "qtran: " << qtran.total() << std::endl;
std::cerr << "btick: " << btick.total() << std::endl;
std::cerr << "rtick: " << rtick.total() << std::endl;
std::cerr << "qorder: " << qorder.total() << std::endl;
#endif

    return 0;
}