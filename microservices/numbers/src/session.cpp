#include "../include/session.hpp"
#include <iostream>
#include <chrono>
#include <boost/json/src.hpp>

namespace chrono = std::chrono;

void fail(beast::error_code ec, char const* what){
	std::cerr << what << ": " << ec.message() << std::endl;;
}

session::session(tcp::socket&& socket)
	: stream_(std::move(socket)) {}

void session::run(){
	net::dispatch(stream_.get_executor(),
		beast::bind_front_handler(&session::do_read, shared_from_this()));
}

void session::do_read(){
	req_ = {};
	stream_.expires_after(std::chrono::seconds(30));
	http::async_read(stream_, buffer_, req_,
		beast::bind_front_handler(&session::on_read, shared_from_this()));
}

void session::on_read(beast::error_code ec, size_t bytes_transferred){
	boost::ignore_unused(bytes_transferred);
	if(ec == http::error::end_of_stream)
		return do_close();
	if(ec)
		return fail(ec, "read");
	send_response(handle_request(std::move(req_)));
}

void session::send_response(http::message_generator&& msg){
	bool keep_alive = msg.keep_alive();
	
	beast::async_write(stream_,
		std::move(msg),
		beast::bind_front_handler(&session::on_write, shared_from_this(), keep_alive));
}

void session::on_write(bool keep_alive, beast::error_code ec, size_t bytes_transferred){
	boost::ignore_unused(bytes_transferred);
	if(ec)
		return fail(ec, "write");
	if(!keep_alive){
		return do_close();
	}
	do_read();
}

void session::do_close(){
	beast::error_code ec;
	stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}


//сделать получения number в виде хеша по времени + 
template <class Body, class Allocator>
http::message_generator session::handle_request(http::request<Body, http::basic_fields<Allocator>>&& req){
    //проверка методов
    std::string_view path = req.target();
    size_t pos = path.find('?');
    if (pos != std::string_view::npos){
        path = path.substr(pos);
    }
    //будут одна ручка и проверка на нее post запросом и в теле пароль

    if (req.method() == http::verb::get && path == "/numbers"){
        boost::json::object obj;
        auto now = std::chrono::system_clock::now();
        // Преобразование в время в секундах
        auto time_since_epoch = std::chrono::system_clock::to_time_t(now);
        // Преобразование в структуру tm
        std::tm* tm = std::localtime(&time_since_epoch);
        // Формирование строки в формате YYYY-MM-DD
        std::stringstream ss, res;
        ss << (tm->tm_year + 1900) << '-'
            << std::setw(2) << std::setfill('0') << (tm->tm_mon + 1) << '-'
            << std::setw(2) << std::setfill('0') << tm->tm_mday;
        //создаем ответ
        obj["date"] = ss.str();

        //number - время в миллисекуqдах
        auto now_number = std::chrono::high_resolution_clock::now();
         // Преобразование в миллисекунды
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now_number.time_since_epoch());
        auto time = std::to_string(milliseconds.count());
        time = time.substr(time.size() - 6, 5);
        obj["number"] = time;
        res << obj;
        
        http::response<http::string_body> response{http::status::ok, 11};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "text/html");
        response.keep_alive(false); //пока false
        response.body() = res.str();
        response.prepare_payload();
        return response;
    }
    return not_found("нет такого");
}

http::message_generator session::bad_request(std::string_view message){
    http::response<http::string_body> res{http::status::bad_request, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false); //пока false
    res.body() = std::string(message);
    res.prepare_payload();
    return res;
}

http::message_generator session::not_found(std::string_view message){
    http::response<http::string_body> res{http::status::not_found, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "The resource '" + std::string(message) + "' was not found.";
    res.prepare_payload();
    return res;
}