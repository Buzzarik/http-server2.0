//"host=app-postgres-test port=5432 dbname=postgres user=user password=1065"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "include/server/listener.hpp"
#include "include/database/database_postgres.hpp"

int main(int argc, char* argv[]){


try{
    // std::shared_ptr<executor_database<executor_database_postgres>> db(new executor_database_postgres("host=app-postgres2.0 port=5432 dbname=postgres user=user password=1065"));
    // if (!db->is_open()){
    //     std::cerr << "Нет бд\n";
    //     return 0;
    // }

    auto const threads = std::max<int>(1, 1);

    net::io_context ioc{threads};

    std::make_shared<listener>(ioc, tcp::endpoint(net::ip::address::from_string("0.0.0.0"), 8080), "host=app-postgres2.0 port=5432 dbname=postgres user=user password=1065")->run();

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc]{
            ioc.run();
        });
    ioc.run();
}
catch(const std::exception& e){
	std::cerr << e.what();
}
return EXIT_SUCCESS;
}