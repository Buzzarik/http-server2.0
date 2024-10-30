#pragma once
#include "../boostbeast.hpp"
#include <pqxx/pqxx>
#include <optional>
#include <string_view>

template <class ExecutorDataBase>
class executor_database{
    public:
        std::optional<json::array> get(json::object&& params){
            return static_cast<ExecutorDataBase*>(this)->get(std::move(params));
        }
        bool post(json::object&& params){
            return static_cast<ExecutorDataBase*>(this)->post(std::move(params));
        }
        bool is_open(){
            return static_cast<ExecutorDataBase*>(this)->is_open();
        }
};