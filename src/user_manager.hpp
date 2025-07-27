#ifndef __USER_MANAGER_HPP
#define __USER_MANAGER_HPP

#include <iostream>
#include <unordered_map>
#include <mutex>

#include "user.hpp"

// TODO thread safe
class UserManager {
private:
    std::mutex mtx;
    std::unordered_map<size_t, User> m_users;
public:
    void erase(size_t id) {
        std::lock_guard<std::mutex> lck(mtx);

        m_users.erase(id);
    }
    void insert(size_t id, User &&user) {
        std::lock_guard<std::mutex> lck(mtx);

        m_users.insert({id, user});
    }
    void insert(size_t id, const User &user) {
        std::lock_guard<std::mutex> lck(mtx);

        std::cout << "const User &user" << std::endl;
        m_users.insert({id, user});
    }

    size_t size() const {
        return m_users.size();
    }

    auto begin() noexcept
    { return m_users.begin(); }
    auto cbegin() const noexcept
    { return m_users.cbegin(); }
    auto end() noexcept
    { return m_users.end();  }
    auto cend() const noexcept
    { return m_users.cend(); }
};

#endif
