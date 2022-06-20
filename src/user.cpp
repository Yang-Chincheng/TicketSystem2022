#include <iostream>

#include "../lib/utility.h"
#include "user.h"

namespace ticket {

int UserManager::add_user(unsigned long opt_idx, const Username &cur_usr_str, const Username &new_usr_str, const Password &pwd, const Name &name, const MailAddr &maddr, int priv ) 
{
    size_t cur_usr = strhasher(cur_usr_str);
    size_t new_usr = strhasher(new_usr_str);
    // when creating the first user
    if(user.empty()) {
        priv = 10;
        user.put(new_usr, UserInfo(pwd, name, maddr, priv), opt_idx, USER_ROLLBACK);
        std::cout << "[" << opt_idx << "] 0\n";
        return 0;
    }
    // user dosen't exist or hasn't logged in
    if(!online[cur_usr_str]) {
        throw user_error("current user doesn't exist or hasn't logged in, failed to create the new user");
    }
    UserInfo cur_info;
    user.get(cur_usr, cur_info);
    // privledge not high enough
    if(priv >= cur_info.pri) {
       throw user_error("current user doesn't have enough privledge, failed to create new account"); 
    }
    // new user already exists
    if(user.count(new_usr)) {
        throw user_error("username already exists, failed to create new account");
    }
    // success
    user.put(new_usr, UserInfo(pwd, name, maddr, priv), opt_idx, USER_ROLLBACK);
    std::cout << "[" << opt_idx << "] 0\n";
    return 0;
}

int UserManager::login(unsigned long opt_idx, const Username &usr_str, const Password &pwd)
{
    size_t usr = strhasher(usr_str);
    // user does not exist
    if(!user.count(usr)) {
        throw user_error("user not found");
    }
    // user have logged in
    if(online[usr_str]) {
        throw user_error("user have logged in");
    }
    UserInfo info;
    user.get(usr, info);
    // wrong password
    if(info.pwd != pwd) {
        throw user_error("wrong password, please try again");
    }
    // success
    online[usr_str] = 1;
    std::cout << "[" << opt_idx << "] 0\n";
    return 0;
}

int UserManager::logout(unsigned long opt_idx, const Username &usr_str) 
{
    size_t usr = strhasher(usr_str);
    // user does not exist or have logged out
    if(!online[usr_str]) {
        throw user_error("user does not exist or have logged out");
    }
    // success
    online[usr_str] = 0;
    std::cout << "[" << opt_idx << "] 0\n";
    return 0;    
}

int UserManager::query_profile(unsigned long opt_idx, const Username &cur_usr_str, const Username &qry_usr_str) 
{
    size_t cur_usr = strhasher(cur_usr_str);
    size_t qry_usr = strhasher(qry_usr_str);
    
    // cur_usr does not exist / hasn't logged in 
    if(!online[cur_usr_str]) {
        throw user_error("current user does not exist or hasn't logged in");
    }
    if(cur_usr == qry_usr) {
        UserInfo info;
        user.get(cur_usr, info);
        std::cout << "[" << opt_idx << "] ";
        std::cout << qry_usr_str << " " << info.name << " " << info.maddr << " " << info.pri << "\n";
        return 0;
    }
    // query user does not exist
    if(!user.count(qry_usr)) {
        throw user_error("user not found");
    }
    UserInfo cur_info, qry_info;
    user.get(cur_usr, cur_info);
    user.get(qry_usr, qry_info);

    // privilege not enough
    if(cur_info.pri <= qry_info.pri) {
        throw user_error("privilege not enough");
    }
    std::cout << "[" << opt_idx << "] ";
    std::cout << qry_usr_str << " " << qry_info.name << " " << qry_info.maddr << " " << qry_info.pri << "\n";    
    return 0;
}

int UserManager::modify_profile(unsigned long opt_idx, const Username &cur_usr_str, const Username &mod_usr_str, const Password &mod_pwd, const Name &mod_name,const MailAddr &mod_maddr, int mod_priv) 
{
    size_t cur_usr = strhasher(cur_usr_str);
    size_t mod_usr = strhasher(mod_usr_str);

    if(!online[cur_usr_str]) {
        throw user_error("current user does not exist or hasn't logged in");
    }
    UserInfo cur_info;
    user.get(cur_usr, cur_info);

    if(cur_usr == mod_usr) {
        if(~mod_priv && mod_priv >= cur_info.pri) {
            throw user_error("privilege not enough");
        }
        UserInfo info(
            mod_pwd? mod_pwd: cur_info.pwd,
            mod_name? mod_name: cur_info.name,
            mod_maddr? mod_maddr: cur_info.maddr,
            ~mod_priv? mod_priv: cur_info.pri
        );
        user.put(cur_usr, info, opt_idx, USER_ROLLBACK);
        std::cout << "[" << opt_idx << "] ";
        std::cout << cur_usr_str << " " << info.name << " " << info.maddr << " " << info.pri << "\n";    
        return 0;
    }
    if(!user.count(mod_usr)) {
        throw user_error("user not found");
    }
    UserInfo mod_info;
    user.get(mod_usr, mod_info);
    if(cur_info.pri <= mod_info.pri || cur_info.pri <= mod_priv) {
        throw user_error("privilege not enough");
    }
    // defualt value refers to no-modification 
    UserInfo info(
        mod_pwd? mod_pwd: mod_info.pwd,
        mod_name? mod_name: mod_info.name,
        mod_maddr? mod_maddr: mod_info.maddr,
        ~mod_priv? mod_priv: mod_info.pri
    );
    user.put(mod_usr, info, opt_idx, USER_ROLLBACK);
    std::cout << "[" << opt_idx << "] ";
    std::cout << mod_usr_str << " " << info.name << " " << info.maddr << " " << info.pri << "\n";    
    return 0;
}

int UserManager::is_online(const Username &user) {
    return online[user];
}

int UserManager::clear_user() {
    online.clear();
    user.clear();
    return 0;
}

int UserManager::rollback_user(unsigned long tstamp) {
    online.clear();
    user.roll_back(tstamp);
    return 0;
}

}