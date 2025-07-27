#include <cstdlib>
#include <iostream>
#include <utility> 

#include "boost_includes.hpp"
#include "session_manager.hpp"


// run in separate thread, pop messages from the queue
// and broadcast to all users
void handle_messages(
        BQueue<Message> &message_queue,
        UserManager &users)
{
    for (;;) {
        auto msg = message_queue.pop();

        for (auto &[id, user]: users) {
            std::cout << "Sending msg to id: " << id << std::endl;
            user.send_message(msg);
        }
    }
}


int main(int argc, char* argv[])
{
    try
    {
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


        BQueue<Message> msg_queue;
        UserManager users;

        SessionManager session_manager(msg_queue, users);

        std::thread(handle_messages,
                std::ref(msg_queue),
                std::ref(users)).detach();

        for(;;)
        {
            tcp::socket socket{ioc};

            acceptor.accept(socket);

            session_manager.create_session(std::move(socket));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
