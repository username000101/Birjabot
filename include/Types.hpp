#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>

#include <Utils.hpp>

namespace birja {
    struct User {
        User(std::int64_t chat_id, std::string username, std::int64_t balance, bool is_god, std::int64_t group_id)
            : chat_id(chat_id), username(username), balance(balance), is_god(is_god), group_id(group_id) {}

        User() = default;


        bool is_god;
        std::string username;
        std::int64_t balance;
        std::int64_t chat_id;
        std::int64_t group_id;

        static User from_json(const std::string& json);

        inline std::string to_json() {
            auto result = fmt::format(R"(
{{
    "user": {{
        "chat_id": "{}",
        "username": "{}",
        "balance": "{}",
        "is_god": {},
        "group_id": "{}"
    }}
}}
)", chat_id, username, balance, is_god, group_id);

            return result;
        }

        bool operator==(User&& other) { return this->chat_id == other.chat_id && this->username == other.username && this->is_god == other.is_god && this->balance == other.balance; }
    };

    struct Result {
        bool ok;
        std::string message;
        std::variant<std::string, std::int64_t, void*, std::vector<std::int64_t>, User> value;
    };

}
