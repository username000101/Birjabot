cmake_minimum_required(VERSION 3.16)

project(BirjaX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

find_package(Boost CONFIG REQUIRED COMPONENTS system json)
find_package(fmt REQUIRED)
find_package(ICU REQUIRED COMPONENTS i18n uc)
find_package(TgBot REQUIRED)

find_library(LMDB_LIBRARY
    NAMES lmdb
    PATHS /usr/lib /usr/local/lib ${CMAKE_PREFIX_PATH}
    DOC "The LMDB library"
)

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
    src/AddGod.cpp
    src/GetUser.cpp
    src/UserFromJson.cpp
    src/PrepareMessage.cpp
    src/OpenWallet.cpp
    src/AddGodCommand.cpp
    src/DemoteGodCommand.cpp
    src/GiveCurrencyCommand.cpp
    src/TransferCurrencyCommand.cpp
    src/ResetWalletCommand.cpp
    src/Convert.cpp
    src/SeizeCurrencyCommand.cpp
    src/GenericHandler.cpp
)

target_include_directories(BirjaBot PRIVATE include)

if (NOT LMDB_LIBRARY)
    message(FATAL_ERROR "Not found the LMDB library... please install it")
else()
    message(NOTICE "Found LMDB at ${LMDB_LIBRARY}")
    target_link_libraries(BirjaBot PRIVATE ${LMDB_LIBRARY})
endif()

target_link_libraries(BirjaBot PUBLIC
    ${Boost_LIBRARIES}
    fmt::fmt
    ${ICU_LIBRARY}
    TgBot::TgBot
)
