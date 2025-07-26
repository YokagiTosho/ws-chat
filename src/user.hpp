#ifndef __USER_HPP
#define __USER_HPP

#include <iostream>

#include "boost_includes.hpp"
#include "message.hpp"


class User {
private:
    std::shared_ptr<websocket::stream<tcp::socket>> m_ws;
    size_t m_id;
    std::string m_name;
    // TODO add 'join_time'
public:
    User(std::shared_ptr<websocket::stream<tcp::socket>> ws, size_t id)
        : m_ws(std::move(ws)), m_id(id), m_name("Anon")
    {
        std::cout << "in User cntr, " << m_ws.use_count() << std::endl;
    }

    User(std::shared_ptr<websocket::stream<tcp::socket>> ws, size_t id, std::string name)
        : m_ws(ws), m_id(id), m_name(name)
    {}

    User(const User &user)
        : m_ws(user.m_ws), m_id(user.m_id), m_name(user.m_name)
    {}

    User(User &&user)
        : m_ws(std::move(user.m_ws)), m_id(user.m_id), m_name(user.m_name)
    {}

    void send_message(const Message &msg) {
        m_ws->text(m_ws->got_text());
        m_ws->write(msg.data());
    }
};

#endif
