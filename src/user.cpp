#include <iostream>

#include "../lib/utility.h"
#include "user.h"

namespace ticket {

std::ostream& operator << (std::ostream& os, const UserPack &pack) {
    os << pack.uid << " " << pack.name << " " << pack.pwd << " " << pack.maddr << " " << pack.pri;
    return os;
}

int UserManager::add_user(
    const Username &cur_usr, const Username &new_usr, 
    const Password &pwd, const Name &name, const MailAddr &maddr, int priv 
) {
    // when creating the first user
    if(user.empty()) {
        priv = 10;
        user.insert(new_usr, UserInfo(pwd, name, maddr, priv));
        return 0;
    }
    // user dosen't exist or hasn't logged in
    if(!online[cur_usr]) {
        throw user_error("current user doesn't exist or hasn't logged in, failed to create the new user");
    }
    UserInfo cur_info;
    user.get(cur_usr, cur_info);
    // privledge not high enough
    if(priv >= cur_info.pri) {
       throw user_error("current user doesn't have enough privledge, failed to create new account"); 
    }
    // new user already exists
    if(user.search(new_usr)) {
        throw user_error("username already exists, failed to create new account");
    }
    // success
    user.insert(new_usr, UserInfo(pwd, name, maddr, priv));
    return 0;
}

int UserManager::login(const Username &usr, const Password &pwd)
{
    // user does not exist
    if(!user.search(usr)) {
        throw user_error("user not found");
    }
    // user have logged in
    if(online[usr]) {
        throw user_error("user have logged in");
    }
    UserInfo info;
    user.get(usr, info);
    // wrong password
    if(info.pwd != pwd) {
        throw user_error("wrong password, please try again");
    }
    // success
    online[usr] = 1;
    return 0;
}

int UserManager::logout(const Username &usr) 
{
    // user does not exist or have logged out
    if(!online[usr]) {
        throw user_error("user does not exist or have logged out");
    }
    // success
    online[usr] = 0;
    return 0;    
}

int UserManager::query_profile(
    const Username &cur_usr, const Username &qry_usr, UserPack &pack
) {
    // cur_usr does not exist / hasn't logged in 
    if(!online[cur_usr]) {
        throw user_error("current user does not exist or hasn't logged in");
    }
    if(cur_usr == qry_usr) {
        UserInfo info;
        user.get(cur_usr, info);
        pack = UserPack(cur_usr, info);
        return 0;
    }
    // query user does not exist
    if(!user.search(qry_usr)) {
        throw user_error("user not found");
    }
    UserInfo cur_info, qry_info;
    user.get(cur_usr, cur_info);
    user.get(qry_usr, qry_info);
    // privilege not enough
    if(cur_info.pri <= qry_info.pri) {
        throw user_error("privilege not enough");
    }
    pack = UserPack(qry_usr, qry_info);
    return 0;
}

int UserManager::modify_profile(
    const Username &cur_usr, const Username &mod_usr, 
    const Password &mod_pwd = "", const Name &mod_name = "",
    const MailAddr &mod_maddr = "", int mod_priv = -1 
) {
    if(!online[cur_usr]) {
        throw user_error("current user does not exist or hasn't logged in");
    }
    UserInfo cur_info;
    user.get(cur_usr, cur_info);
    if(cur_usr == mod_usr) {
        UserInfo info(
            mod_pwd? mod_pwd: cur_info.pwd,
            mod_name? mod_name: cur_info.name,
            mod_maddr? mod_maddr: cur_info.maddr,
            ~mod_priv? mod_priv: cur_info.pri
        );
        user.insert(cur_usr, info);
        return 0;
    }
    if(!user.search(mod_usr)) {
        throw user_error("user not found");
    }
    UserInfo mod_info;
    user.get(mod_usr, mod_info);
    if(cur_info.pri <= mod_info.pri) {
        throw user_error("privilege not enough");
    }
    UserInfo info(
        mod_pwd? mod_pwd: mod_info.pwd,
        mod_name? mod_name: mod_info.name,
        mod_maddr? mod_maddr: mod_info.maddr,
        ~mod_priv? mod_priv: mod_info.pri
    );
    user.insert(mod_usr, info);
    return 0;
}

int UserManager::is_online(const Username &user) {
    return online[user];
}

int UserManager::clear() {
    online.clear(), user.clear();
    return 0;
}

}