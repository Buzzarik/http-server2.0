#include "include/parse.hpp"
#include <memory>
#include <iostream>

int main(){

    std::string_view url1 = "http://localhost:8080/order?date=2024-10-17&cost=100",
                     url2 = "http://localhost:8080/order?cost=100",
                     url3 = "http://localhost:8080/order?cost=100&number=qqqqq&name=banana&from=2020-12-12",
                     url4 = "http://localhost:8080/order?cost=100&number=qqqqq&name=banana&from=2020-12-12&to=2020-12-12&wdxndw=xwxwx",
                     url5 = "fsgdhfjgykuhlij;k",
                     url6 = "http://localhost:8080/order",
                     url7 = "http://localhost:8080/order?cost=100&nu",
                     url8 = "http://localhost:8080/order?cost=100&number=",
                     url9 = "http://localhost:8080/order?cost=100&";
    std::string_view body1 = "{ \
\"receiver\" : \"yandex\", \
\"address\" : \"yandex.com\", \
\"type_pay\" : \"online\", \
\"type_delivered\" : \"office\", \
\"products\" : [ \
    { \
        \"article\" : \"12345\", \
        \"name\" : \"tea\", \
        \"count\" : 10, \
        \"price\" : 100 \
    }, \
    { \
        \"article\" : \"5678\", \
        \"name\" : \"opt\", \
        \"count\" : 5, \
        \"price\" : 50 \
    } \
] \
}";

std::string_view body2 = "\
\"receiver\" : \"yandex\", \
\"address\" : \"yandex.com\", \
\"type_pay\" : \"online\", \
\"type_delivered\" : \"office\", \
\"products\" : [ \
    { \
        \"article\" : \"12345\", \
        \"name\" : \"tea\", \
        \"count\" : 10, \
        \"price\" : 100 \
    }, \
    { \
        \"article\" : \"5678\", \
        \"name\" : \"opt\", \
        \"count\" : 5, \
        \"price\" : 50 \
     \
] \
}";

std::string_view body3 = "[ \
\"receiver\" : \"yandex\", \
\"address\" : \"yandex.com\", \
\"type_pay\" : \"online\", \
\"type_delivered\" : \"office\", \
\"products\" : [ \
    { \
        \"article\" : \"12345\", \
        \"name\" : \"tea\", \
        \"count\" : 10, \
        \"price\" : 100 \
    }, \
    { \
        \"article\" : \"5678\", \
        \"name\" : \"opt\", \
        \"count\" : 5, \
        \"price\" : 50 \
    } \
] \
]";
    std::unique_ptr<parse<parse_query_params>> parse1(new parse_query_params);
    std::unique_ptr<parse<parse_json>> parse2(new parse_json);
    
    int cnt = 1;
    {
        auto res = parse1->to_json(url1);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url2);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url3);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url4);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url5);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url6);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url7);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url8);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse1->to_json(url9);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }

    {
        auto res = parse2->to_json(body1);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse2->to_json(body2);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    {
        auto res = parse2->to_json(body3);
        if (res.has_value()){
            std::cout << *res;
        }
        else{
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    return 0;
}