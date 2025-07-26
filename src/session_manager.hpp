#ifndef __SESSION_MANAGER_HPP
#define __SESSION_MANAGER_HPP

#include <thread>
#include <memory>
#include <utility>

#include "boost_includes.hpp"
#include "message.hpp"
#include "blocking_queue.hpp"
#include "user_manager.hpp"

class SessionManager {
private:
    UserManager &m_users;          // thread safe users manager
    BlockQueue<Message> &m_mqueue; // thread safe blocking queue
    size_t m_id_counter{0};

    std::vector<std::thread> m_sessions;

    void do_session(std::unique_ptr<websocket::stream<tcp::socket>> ws, size_t unique_id) {
        try
        {
            for(;;)
            {
                Message msg;
                msg.read_message(*ws);

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

        m_users.erase(unique_id);
    }
public:
    SessionManager(BlockQueue<Message> &q, UserManager &users)
        : m_mqueue{q}, m_users{users} { }

    void create_session(tcp::socket socket) {
        auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));

        // Set a decorator to change the Server of the handshake
        ws->set_option(websocket::stream_base::decorator(
                    [](websocket::response_type& res)
                    {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-server-sync");
                    }));

        // Accept the websocket handshake
        ws->accept();

        m_users.insert(m_id_counter, {ws, m_id_counter});

        m_sessions.push_back(
                std::thread(
                    &SessionManager::do_session,
                    std::move(ws),
                    m_id_counter));

        m_id_counter++;
    }
};

#endif
