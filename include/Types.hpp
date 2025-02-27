#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>

namespace birja {
    struct User {
        bool is_god;
        std::string username;
        std::int64_t balance;
        std::int64_t chat_id;
        std::int64_t group_id;

        inline std::string to_json() {
            return fmt::format(R"(
{{
    "user": {{
        "chat_id": {},
        "username": "{}",
        "balance": {},
        "is_god": {},
        "group_id": {}
    }}
}}
)", chat_id, username, balance, is_god, group_id);
        }
    };

    struct Result {
        bool ok;
        std::string message;
        std::variant<std::string, std::int64_t, void*, std::vector<std::int64_t>, User> value;
    };

}
