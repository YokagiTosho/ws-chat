#include "user_manager.hpp"

void UserManager::erase(size_t id) {
    std::lock_guard<std::mutex> lck(mtx);

    m_users.erase(id);
}

void UserManager::insert(size_t id, User &&user) {
    std::lock_guard<std::mutex> lck(mtx);

    m_users.insert({id, user});
}

void UserManager::insert(size_t id, const User &user) {
    std::lock_guard<std::mutex> lck(mtx);

    m_users.insert({id, user});
}
