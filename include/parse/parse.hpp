#pragma once
#include "../boostbeast.hpp"
#include <optional>
#include <string_view>
#include <sstream> 

template <class Parse>
class parse{
    public:
        std::optional<json::object> to_json(std::string_view data){
            return static_cast<Parse*>(this)->to_json(data);
        }
        std::string to_string(json::array&& res){
            std::stringstream oss;
            oss << res;
            return oss.str();
        }
};

class parse_json : public parse<parse_json>{
    public:
        std::optional<json::object> to_json(std::string_view data);
};


class parse_query_params : public parse<parse_query_params>{
    public:
        std::optional<json::object> to_json(std::string_view data);
};