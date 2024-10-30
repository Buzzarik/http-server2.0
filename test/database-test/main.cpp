#include "include/database_postgres.hpp"
#include <memory>
#include <iostream>
#include <pqxx/pqxx>
#include <string>



int main(){
    try{
        //короче использовать переменную не в обертке, а просто как поле класса executor_db
        std::unique_ptr<executor_database<executor_database_postgres>> db(new executor_database_postgres("host=app-postgres-test port=5432 dbname=postgres user=user password=1065"));

        if (!db->is_open()){
            std::cerr << "no server" << std::endl;
            return 0;
        }
        //Создание заказов (для теста)
        // for (int i = 10000; i <= 11000; i++){
        //     //number | cost | date | receiver | address | type_pay | type_delivered | article | name | count | price
        //     json::object order;
        //     order["number"] = std::to_string(i);
        //     order["date"] = "2024-" + std::to_string(i % 12 + 1) + "-" + std::to_string(i % 20 + 1);
        //     order["receiver"] = "yandex";
        //     order["address"] = "yandex.ru";
        //     order["type_delivered"] = "busy";
        //     order["type_pay"] = "offline";
        //     json::array products;
        //     json::object obj;
        //     obj["article"] = "qwerer";
        //     obj["name"] = rand() % 2 ? "banana" : "apple";
        //     obj["count"] = 100;
        //     obj["price"] = 10;
        //     products.push_back(obj);
        //     order["products"] = products;
        //     db->post(std::move(order));
        // }

        //by number
        json::object obj1;
        obj1["number"] = "10002";
        auto res1 = db->get(std::move(obj1));
        if (!res1.has_value()){
            std::cerr << "res1 failed" << std::endl;
        }
        else{
            std::cout << *res1 << std::endl;
        }
        //by date by cost
        json::object obj2;
        obj2["date"] = "2024-08-04";
        obj2["cost"] = (int64_t)999;
        auto res2 = db->get(std::move(obj2));
        if (!res2.has_value()){
            std::cerr << "res2 failed" << std::endl;
        }
        else{
            std::cout << *res2 << std::endl;
        }
        
        //by inteval by name
        json::object obj3;
        obj3["from"] = "2024-01-09";
        obj3["to"] = "2024-12-20";
        obj3["name"] = "banana";
        auto res3 = db->get(std::move(obj3));
        if (!res3.has_value()){
            std::cerr << "res3 failed" << std::endl;
        }
        else{
            std::cout << *res3 << std::endl;
        }
    }
    catch(const std::exception& e){
        std::cerr << "whatafak " << e.what() << std::endl;
    }
    return 0;
}