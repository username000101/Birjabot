#pragma once

#include "Types.hpp"

namespace birja {
    namespace db {
        std::int64_t convert_username_to_chatid(std::int64_t group_id, const std::string& username);
        std::string convert_chatid_to_username(std::int64_t group_id, std::int64_t chat_id);

        /* Gods handling */
        Result add_god(std::int64_t group_id, std::int64_t chat_id);
        inline Result add_god(std::int64_t group_id, const std::string& username) { return add_god(group_id, convert_username_to_chatid(group_id, username)); }

        Result demote_god(std::int64_t group_id, std::int64_t chat_id);
        inline Result demote_god(std::int64_t group_id, const std::string& username) { return demote_god(group_id, convert_username_to_chatid(group_id, username)); }

        Result get_gods(std::int64_t group_id);

        /* Users handling */
        Result add_user(std::int64_t group_id, User user);

        bool check_user(std::int64_t group_id, std::int64_t chat_id);
        inline bool check_user(std::int64_t group_id, const std::string& username) { return check_user(group_id, convert_username_to_chatid(group_id, username)); }

        Result get_user(std::int64_t group_id, std::int64_t chat_id);
        inline Result get_user(std::int64_t group_id, const std::string& username) { return get_user(group_id, convert_username_to_chatid(group_id, username)); }

        /* Balance handling */
        Result set_user_balance(std::int64_t group_id, std::int64_t chat_id, std::int64_t value);
        inline Result set_user_balance(std::int64_t group_id, const std::string& username, std::int64_t value) { return set_user_balance(group_id, convert_username_to_chatid(group_id, username), value); }
    }
}
