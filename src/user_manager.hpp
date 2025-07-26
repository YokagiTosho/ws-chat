#ifndef __USER_MANAGER_HPP
#define __USER_MANAGER_HPP

#include <unordered_map>
#include <mutex>

#include "user.hpp"

// TODO make class iterable and thread safe
class UserManager {
private:
    std::unordered_map<size_t, User> m_users;
    std::mutex mtx;
public:
    void erase(size_t id);
    void insert(size_t id, User &&user);
    void insert(size_t id, const User &user);
};

#endif
