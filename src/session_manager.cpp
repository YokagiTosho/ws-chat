#include "session_manager.hpp"

#include <memory>
#include <utility>
#include <iostream>

void SessionManager::do_session(User user) {
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

void SessionManager::create_session(tcp::socket socket) {
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
