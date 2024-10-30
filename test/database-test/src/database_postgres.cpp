#include "../include/database_postgres.hpp"
#include <boost/json/src.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <format>


//number | cost | date | receiver | address | type_pay | type_delivered | article | name | count | price
json::array executor_database_postgres::order_to_json(pqxx::result&& result){
    json::array res;
    int64_t id = -1;
    for (const auto& row : result){
        json::object product;
        product["article"] = row["article"].as<std::string>();
        product["name"] = row["name"].as<std::string>();
        product["count"] = row["count"].as<int64_t>();
        product["price"] = row["price"].as<int64_t>();
        if (id != -1 && id == row["id"].as<int64_t>()){
            res.back().as_object()["products"].as_array().push_back(product);
        }
        else{
            id = row["id"].as<int64_t>();
            json::object order;
            order["number"] = row["number"].as<std::string>();
            order["cost"] = row["cost"].as<int64_t>();
            order["date"] = row["date"].as<std::string>();
            order["receiver"] = row["receiver"].as<std::string>();
            order["address"] = row["address"].as<std::string>();
            order["type_pay"] = row["type_pay"].as<std::string>();
            order["type_delivered"] = row["type_delivered"].as<std::string>();
            order["products"] = json::array();
            order["products"].as_array().push_back(product);
            res.push_back(order);
        }
    }
    return res;
}


executor_database_postgres::executor_database_postgres(std::string_view config)
    :conn_(std::string(config).c_str())
{}

std::optional<json::array> executor_database_postgres::get(json::object&& params){
    //сверяем данные
    if (check_by_number(params)){
        return get_by_number(std::move(params));
    }
    if (check_by_date_cost(params)){
        return get_by_date_cost(std::move(params));
    }
    if (check_by_interval_name(params)){
        return get_by_interval_name(std::move(params));
    }
    std::cerr << "Неверные параметры для запроса" << std::endl;
    return {};
}

bool executor_database_postgres::post(json::object&& params){
    //сверяем данные
    if (check_create_order(params)){
        return post_create_order(std::move(params));
    }
    std::cerr << "Неверные параметры для создания заказа" << std::endl;
    return false;
}

bool executor_database_postgres::is_open(){
    return conn_.is_open();
}

std::optional<json::array> executor_database_postgres::get_by_number(json::object&& params){
    if (!is_open()){
        std::cerr << "Нет соединения с БД" << std::endl;
        return {};
    }
    constexpr std::string_view query_by_number = "SELECT a.id, \"number\", cost, date, receiver, address, type_pay, type_delivered, article, name, count, price FROM \"Orders\" AS a \
JOIN \"DetailsOrder\" AS b \
ON a.id = b.id_order \
WHERE \"number\" = '{}' \
ORDER BY a.id ASC";
    pqxx::work t(conn_);
    try{
        //переделать все пож prepare  и пофиксить баллы
        auto result = t.exec(std::format(query_by_number, std::string(params["number"].as_string())));
        // auto result = t.exec_prepared("get_by_number", params["number"].as_string());
        //надо сделать обработку, чтобы вывод был красивым (заказ + продукты) (ну или забить на это и просто для начала сделать заглушку)
        json::array res = order_to_json(std::move(result));
        t.commit();
        return {res};
    }
    catch(const std::exception& e){
        std::cerr << "Ошибка в запросе " << e.what() << std::endl;
        t.abort();
    }
    return {};
}

std::optional<json::array> executor_database_postgres::get_by_date_cost(json::object&& params){
    if (!is_open()){
        std::cerr << "Нет соединения с БД" << std::endl;
        return {};
    }
    constexpr std::string_view query_by_date_cost = "SELECT a.id, \"number\", cost, date, receiver, address, type_pay, type_delivered, article, name, count, price FROM \"Orders\" AS a \
JOIN \"DetailsOrder\" AS b \
ON a.id = b.id_order \
WHERE date = '{}' AND cost > {} \
ORDER BY a.id ASC";
    pqxx::work t(conn_);
    try{
        auto result = t.exec(std::format(query_by_date_cost, std::string(params["date"].as_string()), params["cost"].as_int64()));
        // auto result = t.exec_prepared("get_by_date_cost", params["date"].as_string(), params["cost"].as_int64());
        //надо сделать обработку, чтобы вывод был красивым (заказ + продукты) (ну или забить на это и просто для начала сделать заглушку)
        json::array res = order_to_json(std::move(result));
        t.commit();
        return {res};
    }
    catch(const std::exception& e){
        std::cerr << "Ошибка в запросе " << e.what() << std::endl;
        t.abort();
    }
    return {};
}

std::optional<json::array> executor_database_postgres::get_by_interval_name(json::object&& params){
    if (!is_open()){
        std::cerr << "Нет соединения с БД" << std::endl;
        return {};
    }
    constexpr std::string_view query_by_interval_name = "SELECT a.id, \"number\", cost, date, receiver, address, type_pay, type_delivered, article, name, count, price FROM \"Orders\" AS a \
JOIN \"DetailsOrder\" AS b \
ON a.id = b.id_order \
WHERE date BETWEEN '{}' AND '{}' AND a.id NOT IN (SELECT id_order \
FROM \"DetailsOrder\" WHERE name = '{}') \
ORDER BY a.id ASC";
    pqxx::work t(conn_);
    try{
        auto result = t.exec(std::format(query_by_interval_name, std::string(params["from"].as_string()), std::string(params["to"].as_string()), std::string(params["name"].as_string())));
        // auto result = t.exec_prepared("get_by_interval_name", params["from"].as_string(), params["to"].as_string(), params["name"].as_string());
        //надо сделать обработку, чтобы вывод был красивым (заказ + продукты) (ну или забить на это и просто для начала сделать заглушку)
        json::array res = order_to_json(std::move(result));
        t.commit();
        return {res};
    }
    catch(const std::exception& e){
        std::cerr << "Ошибка в запросе " << e.what() << std::endl;
        t.abort();
    }
    return {};
}

bool executor_database_postgres::post_create_order(json::object&& params){
    if (!is_open()){
        std::cerr << "Нет соединения с БД" << std::endl;
        return {};
    }
    constexpr std::string_view query1 = "INSERT INTO \"Orders\"(\"number\", cost, date, receiver, address, type_pay, type_delivered) \
VALUES ('{}', {}, '{}', '{}', '{}', '{}', '{}') RETURNING id";
    constexpr std::string_view query2 = "INSERT INTO \"DetailsOrder\"( \
article, name, count, price, id_order) \
VALUES ('{}', '{}', {}, {}, {})";
    pqxx::work t(conn_);
    try{
        //number | cost | date | receiver | address | type_pay | type_delivered | article | name | count | price
        int64_t cost = 0;
        for (const auto& product : params.at("products").as_array()){
            cost += product.as_object().at("count").as_int64() * product.as_object().at("price").as_int64();
        }
        auto result = t.exec(std::format(query1, std::string(params["number"].as_string()), cost, std::string(params["date"].as_string()), std::string(params["receiver"].as_string()),
                            std::string(params["address"].as_string()), std::string(params["type_pay"].as_string()), std::string(params["type_delivered"].as_string())));
        // auto result = t.exec_prepared("create_order", params["number"].as_string(), cost, params["date"].as_string(), params["receiver"].as_string(),
        //                     params["address"].as_string(), params["type_pay"].as_string(), params["type_delivered"].as_string());
        int64_t id = result[0]["id"].as<int64_t>();

        for (const auto& product : params.at("products").as_array()){
            auto obj = product.as_object();
            t.exec(std::format(query2, std::string(obj.at("article").as_string()), std::string(obj.at("name").as_string()), obj.at("count").as_int64(), obj.at("price").as_int64(), id));
            // t.exec_prepared("create_details_order", obj.at("article").as_string(), obj.at("name").as_string(), obj.at("count").as_int64(), obj.at("price").as_int64(), id);
        }
        t.commit();
        return true;
    }
    catch(const std::exception& e){
        std::cerr << "Ошибка в запросе " << e.what() << std::endl;
        t.abort();
    }
    return false;
}

bool executor_database_postgres::check_by_number(const json::object& params){
    return params.size() == 1 && params.contains("number") && params.at("number").is_string();
}

bool executor_database_postgres::check_by_date_cost(const json::object& params){
    return params.size() == 2 && params.contains("date") && params.at("date").is_string()
            && params.contains("cost") && params.at("cost").is_int64();
}

bool executor_database_postgres::check_by_interval_name(const json::object& params){
    return params.size() == 3 && params.contains("from") && params.at("from").is_string()
            && params.contains("to") && params.at("to").is_string()
            && params.contains("name") && params.at("name").is_string();
}

bool executor_database_postgres::check_create_order(const json::object& params){
    if (!(params.size() == 7 && params.contains("number") && params.at("number").is_string()
            && params.contains("date") && params.at("date").is_string()
            && params.contains("receiver") && params.at("receiver").is_string()
            && params.contains("address") && params.at("address").is_string()
            && params.contains("type_pay") && params.at("type_pay").is_string()
            && params.contains("type_delivered") && params.at("type_delivered").is_string()
            && params.contains("products") && params.at("products").is_array())){
                return false;
    }

    for (const auto& product : params.at("products").as_array()){
        if (!(product.is_object())){
            std::cerr << "1" << std::endl;
            return false;
        }
        const auto obj = product.as_object();
        if (!(obj.size() == 4 && obj.contains("article") && obj.at("article").is_string()
                && obj.contains("name") && obj.at("name").is_string()
                && obj.contains("count") && obj.at("count").is_int64()
                && obj.contains("price") && obj.at("price").is_int64())){
            std::cerr << "2" << std::endl;
            return false;
        }
    }
    return true;
}