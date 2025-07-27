#ifndef __USER_HPP
#define __USER_HPP

#include "boost_includes.hpp"
#include "message.hpp"

class User {
private:
    std::shared_ptr<websocket::stream<tcp::socket>> m_ws;
    size_t m_id;
    std::string m_name;
    // TODO add 'join_time'
public:
    User(const User &user)
        : m_ws(user.m_ws), m_id(user.m_id), m_name(user.m_name) {}

    User(User &&user)
        : m_ws(std::move(user.m_ws)), m_id(user.m_id), m_name(std::move(user.m_name)) {}

    User(std::shared_ptr<websocket::stream<tcp::socket>> ws, size_t id)
        : m_ws(ws), m_id(id), m_name("Anon") {}

    User(std::shared_ptr<websocket::stream<tcp::socket>> ws, size_t id, std::string name)
        : m_ws(ws), m_id(id), m_name(name) {}

    size_t id() { return m_id; }

    void send_message(const Message &);
    Message receive_message();
};

#endif
