#pragma once
#include "session.hpp"
#include "../database/database_postgres.hpp"

// template <class ExecutorDataBase>
class listener : public std::enable_shared_from_this<listener>
{
public:
    listener(net::io_context& ioc, tcp::endpoint endpoint, std::string_view config_db);
    void run();

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::string config_db_;

    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);

};