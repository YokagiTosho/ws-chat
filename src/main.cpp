#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/src.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <memory>

#include "blocking_queue.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

//------------------------------------------------------------------------------

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

class User {
private:
    std::shared_ptr<websocket::stream<tcp::socket>> m_ws;
    std::string m_name;
    // TODO add 'join_time'
public:
    User(std::shared_ptr<websocket::stream<tcp::socket>> ws)
        : m_ws(std::move(ws)), m_name("Anon") {std::cout << "in User cntr, " << m_ws.use_count() << std::endl; }

    User(std::shared_ptr<websocket::stream<tcp::socket>> ws, std::string name)
        : m_ws(ws), m_name(name) {}

    User(const User &user)
        : m_ws(user.m_ws), m_name(user.m_name) {}

    User(User &&user) : m_ws(std::move(user.m_ws)), m_name(user.m_name) {}

    void send_message(const Message &msg) {
        m_ws->text(m_ws->got_text());
        m_ws->write(msg.data());
    }
};

std::vector<User> users;
BlockQueue<Message> message_queue; // thread safe blocking queue

void
do_session(tcp::socket socket)
{
    try
    {
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

        users.push_back(ws);

        for(;;)
        {
            Message msg;
            msg.read_message(*ws);

            message_queue.push(std::move(msg));
        }
    }
    catch(beast::system_error const& se)
    {
        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// run in separate thread, pop messages from the queue
// and broadcast to all users
void handle_messages() {
    for (;;) {
        auto msg = message_queue.pop();
        std::cout << msg;

        for (auto &user : users) {
            user.send_message(msg);
        }
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
        if (argc != 3)
        {
            std::cerr <<
                "Usage: ws-chat <address> <port>\n" <<
                "Example:\n" <<
                "    ws-chat 127.0.0.1 8080\n";
            return EXIT_FAILURE;
        }
        auto const address = net::ip::make_address(argv[1]);
        auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

        net::io_context ioc{1};

        tcp::acceptor acceptor{ioc, {address, port}};

        std::thread(handle_messages).detach();

        for(;;)
        {
            tcp::socket socket{ioc};

            acceptor.accept(socket);

            std::thread(
                &do_session,
                std::move(socket)).detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
