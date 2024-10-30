#pragma once
#include "session.hpp"

class listener : public std::enable_shared_from_this<listener>
{
public:
    listener(net::io_context& ioc, tcp::endpoint endpoint);
    void run();

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
};