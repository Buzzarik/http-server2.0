#include "../../include/server/session.hpp"
#include <future>
#include <chrono>
// #include <boost/json/src.hpp>


std::optional<json::object> longRunningTask() {
    try{
        net::io_context ioc;
        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);
        std::string host = "app-numbers2.0";
        std::string port = "8081";
        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);
        std::cerr <<  "удалось подключиться к number\n";

        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, "/numbers", 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        
        // Send the HTTP request to the remote host
        http::write(stream, req);
        std::cerr << "запроса отправлен\n";
        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        boost::beast::error_code er;
        stream.socket().shutdown(tcp::socket::shutdown_both, er);

        std::cerr << "данные с сервера number приняты\n";
        // Вывод статуса ответа
        boost::system::error_code ec;
        std::stringstream ss;
        auto data = boost::beast::buffers_to_string(res.body().data());
        auto value = json::parse(data, ec);
        if (ec || !value.is_object()){
            return {};
        }
        auto ans = value.as_object();
        std::cerr << ans << "\n";
        return {ans};
    }
    catch (const boost::system::system_error& e){
        std::cerr << e.what() << "\n";
        return {};
    }

}

// template<class ExecutorDataBase>
std::optional<json::object> session::request_by_number(){
  std::future<std::optional<json::object>> taskFuture = std::async(std::launch::async, longRunningTask);
  // Устанавливаем таймаут для задачи
  std::chrono::milliseconds timeout(1000); // 3 секунды

  // Ждем завершения задачи или таймаута
  if (taskFuture.wait_for(timeout) == std::future_status::timeout) {
    return {}; //время истекло
  } 
  else {
    // Получаем и выводим результат задачи
    return taskFuture.get();
    //std::cout << "Результат задачи: " << result << std::endl;
  }
}

void fail(beast::error_code ec, char const* what){
	std::cerr << what << ": " << ec.message() << "\n";
}

// template<class ExecutorDataBase>
session::session(tcp::socket&& socket, std::string_view config_db)
	: stream_(std::move(socket))
	, config_db_(config_db) 
{
    executor_.reset(new executor_database_postgres(config_db_));
}

// template<class ExecutorDataBase>
void session::run(){
	net::dispatch(stream_.get_executor(),
		beast::bind_front_handler(&session::do_read, shared_from_this()));
}

// template<class ExecutorDataBase>
void session::do_read(){
	req_ = {};
	stream_.expires_after(std::chrono::seconds(30));
	http::async_read(stream_, buffer_, req_,
		beast::bind_front_handler(&session::on_read, shared_from_this()));
}

// template<class ExecutorDataBase>
void session::on_read(beast::error_code ec, size_t bytes_transferred){
	boost::ignore_unused(bytes_transferred);
	if(ec == http::error::end_of_stream)
		return do_close();
	if(ec)
		return fail(ec, "read");
	send_response(handle_request(std::move(req_)));
}

// template<class ExecutorDataBase>
void session::send_response(http::message_generator&& msg){
	bool keep_alive = msg.keep_alive();
	
	beast::async_write(stream_,
		std::move(msg),
		beast::bind_front_handler(&session::on_write, shared_from_this(), keep_alive));
}

// template<class ExecutorDataBase>
void session::on_write(bool keep_alive, beast::error_code ec, size_t bytes_transferred){
	boost::ignore_unused(bytes_transferred);
	if(ec)
		return fail(ec, "write");
	if(!keep_alive){
		return do_close();
	}
	do_read();
}

// template<class ExecutorDataBase>
void session::do_close(){
	beast::error_code ec;
	stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}


template<class Body, class Allocator>
http::message_generator session::handle_request(http::request<Body, http::basic_fields<Allocator>>&& req){
    //проверка методов
    std::string_view url = req.target();
    size_t pos = url.find('?');
    std::string_view path = url.substr(0, pos);
    std::string_view params = url.substr(pos + 1);
    switch (req.method())
    {
    case http::verb::get :
        //проверяем вхождение ручек
        if (!Endpoints::contains_enpoint(http::verb::get, std::string(path))){
            std::cerr << "нет такой ручки GET - " << url << "\n";
            return not_found("нет такого запроса");
        }
        return get_request(url);
        break;
    case http::verb::post :
        //проверяем вхождение ручек
        if (!Endpoints::contains_enpoint(http::verb::post, std::string(path))){
            std::cerr << "нет такой ручки GET - " << url << "\n";
            return not_found("нет такого запроса");
        }
        return post_request(req.body());
        break;
    default:
        return not_found("нет такого запроса");
        break;
    }
}

// template<class ExecutorDataBase>
http::message_generator session::get_request(std::string_view url){
    std::unique_ptr<parse<parse_query_params>> parse(new parse_query_params());

    auto params = parse->to_json(url);
    if (!params.has_value()){
        return bad_request("Некоректные параметры запроса");
    }
    auto result = executor_->get(std::move(*params));
    if (result.has_value()){
        http::response<http::string_body> res{http::status::ok, 11};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(false); //пока false
        res.body() = parse->to_string(std::move(*result));
        res.prepare_payload();
        return res;
    }

    return bad_request("неверный набор параметров");
}

// template<class ExecutorDataBase>
http::message_generator session::post_request(std::string_view body){
    auto data_numbers = request_by_number();
    if (!data_numbers.has_value()){
        std::cerr << "сервер number не отвечает\n";
        return server_error("сервер не отвечает");
    }
    std::cerr << "идем дальше по созданию 1\n";
    auto data = *data_numbers;

    std::unique_ptr<parse<parse_json>> parse(new parse_json());
    auto data_body_opt = parse->to_json(body);
    if (!data_body_opt.has_value()){
        std::cerr << "post запрос неправильный\n";
        return bad_request("неправильный запрос на создание, не JSON");
    }
    auto data_body = *data_body_opt;
    for (const auto& d : data){
        data_body[d.key()] = d.value();
    }

    std::cerr << data_body << std::endl;

    if (executor_->post(std::move(data_body))){
        http::response<http::string_body> res{http::status::created, 11};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(false); //пока false
        res.body() = "";
        res.prepare_payload();
        return res;
    }
    return bad_request("Неправильные параметры в post запросе");
}

// template<class ExecutorDataBase>
http::message_generator session::bad_request(std::string_view message){
    http::response<http::string_body> res{http::status::bad_request, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false); //пока false
    res.body() = std::string(message);
    res.prepare_payload();
    return res;
}

// template<class ExecutorDataBase>
http::message_generator session::not_found(std::string_view message){
    http::response<http::string_body> res{http::status::not_found, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "The resource '" + std::string(message) + "' was not found.";
    res.prepare_payload();
    return res;
}

// template<class ExecutorDataBase>
http::message_generator session::server_error(std::string_view message){
    http::response<http::string_body> res{http::status::internal_server_error, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "An error occurred: '" + std::string(message) + "'";
    res.prepare_payload();
    return res;
}