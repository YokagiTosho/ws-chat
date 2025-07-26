#ifndef __MESSAGE_HPP
#define __MESSAGE_HPP

#include "boost_includes.hpp"

class Message {
    beast::flat_buffer m_buffer;
public:
    void read_message(websocket::stream<tcp::socket> &ws) {
        ws.read(m_buffer);
    }

    Message() {}
    Message(Message &&msg) : m_buffer(std::move(msg.m_buffer)) {}
    Message(const Message &msg) : m_buffer(msg.m_buffer) {}

    auto data() const {
        return m_buffer.data();
    }

    friend std::ostream &operator<<(std::ostream& os, const Message& msg)
    {
        os << beast::buffers_to_string(msg.m_buffer.data());
        return os;
    }
};

#endif
