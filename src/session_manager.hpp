#ifndef __SESSION_MANAGER_HPP
#define __SESSION_MANAGER_HPP

#include <thread>

#include "boost_includes.hpp"
#include "message.hpp"
#include "blocking_queue.hpp"
#include "user_manager.hpp"

class SessionManager {
private:
    UserManager &m_users;          // thread safe users manager
    BQueue<Message> &m_mqueue; // thread safe blocking queue
    size_t m_id_counter{0};        // unique id for each user

    std::vector<std::thread> m_sessions;

    void do_session(User user);
public:
    SessionManager(BQueue<Message> &q, UserManager &users)
        : m_mqueue{q}, m_users{users} { }

    void create_session(tcp::socket socket);
};

#endif
