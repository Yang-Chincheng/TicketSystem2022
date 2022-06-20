#ifndef _TICKET_SYSTEM_USR_H_
#define _TICKET_SYSTEM_USR_H_

#define USER_ROLLBACK 1

#include "../lib/utility.h"
#include "../lib/cached_bptree.h"
#include "../lib/hashmap.h"

namespace ticket {

/**
 * @brief a class recording the profile of users 
 * username is omitted
 */
struct UserInfo {
    Password pwd;
    Name name;
    MailAddr maddr;
    int pri;

    UserInfo() = default;
    UserInfo(const UserInfo &o) = default;
    UserInfo(
        const Password &ps, 
        const Name &nm, 
        const MailAddr &ma, 
        int pr
    ): pwd(ps), name(nm), maddr(ma), pri(pr) {}

};

class UserManager {
protected:
    // profiles of all users
    cached_bptree<size_t, UserInfo> user;
    // log-in status of all users
    hashmap<Username, bool, StrHasher> online;

    int is_online(const Username &user);

    int clear_user();

    int rollback_user(unsigned long opt_idx);

public:
    UserManager(): user("user"), online() {}
    UserManager(const UserManager &o) = delete;
    ~UserManager() = default;

    int add_user(
        unsigned long opt_idx,
        const Username &cur_usr, 
        const Username &new_usr, 
        const Password &pwd, 
        const Name &name,
        const MailAddr &maddr,
        int priv 
    );

    int login(
        unsigned long opt_idx,
        const Username &usr, 
        const Password &pwd
    );

    int logout(
        unsigned long opt_idx,
        const Username &usr
    );

    int query_profile(
        unsigned long opt_idx,
        const Username &cur_usr,
        const Username &qry_usr
    );

    int modify_profile(
        unsigned long opt_idx,
        const Username &cur_usr, 
        const Username &mod_usr, 
        const Password &mod_pwd, 
        const Name &mod_name,
        const MailAddr &mod_maddr,
        int mod_priv
    );

};

}

#endif