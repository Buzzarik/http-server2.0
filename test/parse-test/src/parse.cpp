#include "../include/parse.hpp"
#include <iostream>
#include <boost/json/src.hpp>

std::optional<json::object> parse_json::to_json(std::string_view data){
    boost::system::error_code ec;
    auto value = boost::json::parse(data, ec);
    if (ec || !value.is_object()){
        return {};
    }
    return {value.as_object()};
}


std::optional<json::object> parse_query_params::to_json(std::string_view data){
    size_t pos = data.find('?');
    data = data.substr(pos + 1);
    if (data.empty()){
        return {};
    }
    json::object res;
    std::vector<std::string_view> params;
    try{
        boost::algorithm::split(params, data, boost::algorithm::is_any_of("&"));
        for (auto param : params){
            size_t pos_equal = param.find('=');
            if (pos_equal == 0 || pos_equal == param.size() - 1 || pos_equal == std::string_view::npos){
                return {};
            }
            std::string key = std::string(param.substr(0, pos_equal));
            std::string value = std::string(param.substr(pos_equal + 1));
            //в ТЗ только cost должен быть числом для запроса
            if (key == "cost"){
                int64_t val = std::stoll(value.c_str());
                res[key] = val;
            }
            else{
                res[key] = value;
            }
        }
    }
    catch (const std::exception& e){
        return {};
    }
    return {res};
}