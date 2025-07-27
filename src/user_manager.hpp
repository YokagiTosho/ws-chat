#ifndef __USER_MANAGER_HPP
#define __USER_MANAGER_HPP

#include <unordered_map>
#include <mutex>

#include "user.hpp"

class UserManager {
private:
    std::mutex mtx;
    std::unordered_map<size_t, User> m_users;
public:
    void erase(size_t);
    void insert(size_t, User &&);
    void insert(size_t, const User &);

    size_t size() const { return m_users.size(); }

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
