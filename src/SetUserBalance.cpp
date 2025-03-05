#include "DBUtils.hpp"

#include <boost/json.hpp>

#include "Utils.hpp"

birja::Result birja::db::set_user_balance(std::int64_t group_id, std::int64_t chat_id, std::int64_t value) {
    auto user_result = get_user(group_id, chat_id);
    if (!user_result.ok)
        return {false, user_result.message, nullptr};

    auto user = std::get<birja::User>(user_result.value);
    user.balance = value;

    auto update_result = add_user(group_id, user);
    if (!update_result.ok) {
        LOG(__FUNCTION__, ": Failed to set user(", chat_id, ") balance: ", update_result.message);
        return {false, update_result.message, nullptr};
    }

    return {true, "", value};
}
