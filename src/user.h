#ifndef _TICKET_SYSTEM_USR_H_
#define _TICKET_SYSTEM_USR_H_

#include "../lib/utility.h"
#include "../test/bptree.h"
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
    UserInfo(const Password &ps, const Name &nm, const MailAddr &ma, int pr)
    : pwd(ps), name(nm), maddr(ma), pri(pr) {}
    UserInfo(const UserInfo &o) = default;
    friend std::ostream& operator << (std::ostream& os, const UserInfo &usr);

};

class UserManager {
private:
    bptree<Username, UserInfo> user;
    hashmap<Username, bool, StrHasher> online;
public:
    UserManager(): user("user"), online() {}
    UserManager(const UserManager &o) = delete;
    ~UserManager() = default;

    int add_user(
        const Username &cur_usr, 
        const Username &new_usr, 
        const Password &pwd, 
        const Name &name,
        const MailAddr &maddr,
        int priv 
    );

    int login(
        const Username &usr, 
        const Password &pwd
    );

    int logout(
        const Username &usr
    );

    int query_profile(
        const Username &cur_usr,
        const Username &qry_usr,
        UserInfo &info
    );

    int modify_profile(
        const Username &cur_usr, 
        const Username &mod_usr, 
        const Password &mod_pwd, 
        const Name &mod_name,
        const MailAddr &mod_maddr,
        int mod_priv 
    );

    int clear();
};

}

#endif