#include "user.hpp"

void User::send_message(const Message &msg) {
    m_ws->text(m_ws->got_text());
    m_ws->write(msg.data());
}

Message User::receive_message() {
    Message msg;
    msg.read_message(*m_ws);
    return msg;
}

