#pragma once
#include "database.hpp"
#include <string_view>
#include <pqxx/pqxx>
#include <memory>

class executor_database_postgres: public executor_database<executor_database_postgres>{
    public:
        executor_database_postgres(std::string_view config);
        std::optional<json::array> get(json::object&& params);
        bool post(json::object&& params);
        bool is_open();
    private:
        std::optional<json::array> get_by_number(json::object&& params);
        std::optional<json::array> get_by_date_cost(json::object&& params);
        std::optional<json::array> get_by_interval_name(json::object&& params);
        bool post_create_order(json::object&& params);
        bool check_by_number(const json::object& params);
        bool check_by_date_cost(const json::object& params);
        bool check_by_interval_name(const json::object& params);
        bool check_create_order(const json::object& params);
        json::array order_to_json(pqxx::result&& result);
        pqxx::connection conn_;
};