cmake_minimum_required(VERSION 3.16)

project(BirjaX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG REQUIRED COMPONENTS system locale json)
find_package(fmt REQUIRED)
find_package(RocksDB REQUIRED)
find_package(TgBot REQUIRED)

if (NOT DEFINED BOT_TOKEN)
    message(FATAL_ERROR "Specify the BOT_TOKEN")
else()
    add_compile_definitions(TOKEN="${BOT_TOKEN}")
endif()

if (NOT DEFINED DB_PATH)
    message(FATAL_ERROR "Specify the DB_PATH")
else()
    add_compile_definitions(DBPATH="${DB_PATH}")
endif()

include_directories(include)

add_executable(BirjaBot
    main.cpp

    include/Types.hpp
    include/DBUtils.hpp
    src/GetGods.cpp
    include/Utils.hpp
    src/AddGod.cpp
    src/AddUser.cpp
    src/DemoteGod.cpp
    src/SetUserBalance.cpp
    include/Command.hpp
    include/Bot.hpp
    src/BotStart.cpp
    src/CheckUser.cpp
)



target_link_libraries(BirjaBot PUBLIC
    ${Boost_LIBRARIES}
    fmt::fmt
    RocksDB::rocksdb
    TgBot::TgBot
)
