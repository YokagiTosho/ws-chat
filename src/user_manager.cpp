#include "user_manager.hpp"

#include <utility>

bool UserManager::erase(size_t id) {
    std::lock_guard<std::mutex> lck(mtx);

    return m_users.erase(id) > 0;
}

bool UserManager::insert(size_t id, User &&user) {
    std::lock_guard<std::mutex> lck(mtx);

    return m_users.try_emplace(id, std::forward<User>(user)).second;
}

bool UserManager::insert(size_t id, const User &user) {
    std::lock_guard<std::mutex> lck(mtx);

    return m_users.try_emplace(id, std::cref(user)).second;
}
