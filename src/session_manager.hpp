#ifndef __SESSION_MANAGER_HPP
#define __SESSION_MANAGER_HPP

#include <thread>
#include <memory>
#include <utility>
#include <iostream>

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

    void do_session(User user) {
        try
        {
            for(;;)
            {
                auto msg = user.receive_message();
                m_mqueue.push(std::move(msg));
            }
        }
        catch(beast::system_error const& se)
        {
            if(se.code() != websocket::error::closed) {
                std::cerr << "Error: " << se.code().message() << std::endl;
            }
        }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        m_users.erase(user.id());
    }

public:
    SessionManager(BQueue<Message> &q, UserManager &users)
        : m_mqueue{q}, m_users{users} { }

    void create_session(tcp::socket socket) {
        auto ws = std::make_unique<websocket::stream<tcp::socket>>(std::move(socket));

        // Set a decorator to change the Server of the handshake
        ws->set_option(websocket::stream_base::decorator(
                    [](websocket::response_type& res)
                    {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " ws-chat");
                    }));

        // Accept the websocket handshake
        ws->accept();

        User user{std::move(ws), m_id_counter};

        m_users.insert(m_id_counter, user);

        std::thread(
            &SessionManager::do_session,
            this,
            std::move(user)
            ).detach();

        m_id_counter++;
    }
};

#endif
