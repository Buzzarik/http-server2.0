#pragma once
#include "../boostbeast.hpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "../database/database_postgres.hpp"
#include "../../include/parse/parse.hpp"
#include "schema.hpp"

void fail(beast::error_code ec, char const* what);

// template<class ExecutorDataBase>
class session : public std::enable_shared_from_this<session>{
public:
	session(tcp::socket&& socket, std::string_view config_db);
	void run();
private:
	void do_read();
	void on_read(beast::error_code ec, size_t bytes_transferred);
	void send_response(http::message_generator&& res);
	void on_write(bool keep_alive, beast::error_code ec, size_t bytes_transferred);
	void do_close();
	
	template <class Body, class Allocator>
	http::message_generator handle_request(http::request<Body, http::basic_fields<Allocator>>&& req);
	http::message_generator get_request(std::string_view url);
	http::message_generator post_request(std::string_view body);
	http::message_generator bad_request(std::string_view message);
	http::message_generator not_found(std::string_view message);
	http::message_generator server_error(std::string_view message);
	std::optional<json::object> request_by_number();

	beast::tcp_stream stream_;
	beast::flat_buffer buffer_;
	http::request<http::string_body> req_;
	std::string config_db_;
	std::shared_ptr<executor_database<executor_database_postgres>> executor_;
};