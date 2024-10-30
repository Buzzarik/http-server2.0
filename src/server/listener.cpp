#include "../../include/server/listener.hpp"
#include "../../include/database/database_postgres.hpp"
	
// template<class ExecutorDataBase>
listener::listener(net::io_context& ioc, tcp::endpoint endpoint, std::string_view config_db)
	: ioc_(ioc)
	, acceptor_(net::make_strand(ioc))
	, config_db_(config_db)
{
	beast::error_code ec;
	acceptor_.open(endpoint.protocol(), ec);
	if(ec){
		fail(ec, "open");
		return;
	}
	acceptor_.set_option(net::socket_base::reuse_address(true), ec);
	if(ec){
		fail(ec, "set_option");
		return;
	}
	acceptor_.bind(endpoint, ec);
	if(ec)
	{
		fail(ec, "bind");
		return;
	}
	acceptor_.listen(
	net::socket_base::max_listen_connections, ec);
	if(ec)
	{
		fail(ec, "listen");
		return;
	}
}

// template<class ExecutorDataBase>
void listener::run(){
	do_accept();
}

// template<class ExecutorDataBase>
void listener::do_accept(){
	acceptor_.async_accept(
		net::make_strand(ioc_),
		beast::bind_front_handler(&listener::on_accept, shared_from_this()));
}

// template<class ExecutorDataBase>
void listener::on_accept(beast::error_code ec, tcp::socket socket)
{
	if(ec)
	{
		fail(ec, "accept");
		return; // To avoid infinite loop
	}
	else{
		std::make_shared<session>(std::move(socket), config_db_)->run();
	}
	do_accept();
}