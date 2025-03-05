#include "DBUtils.hpp"

#include <boost/json.hpp>

#include "Utils.hpp"

birja::Result birja::db::demote_god(std::int64_t group_id, std::int64_t chat_id) {
    auto gods = db::get_gods(group_id);
    if (!gods.ok)
        return {false, gods.message, nullptr};

    auto gods_v = std::get<std::vector<std::int64_t>>(gods.value);
    if (std::find(gods_v.begin(), gods_v.end(), chat_id) == gods_v.end())
        return {true, "Пользователь " + convert_chatid_to_username(group_id, chat_id) + " не является богом", nullptr};
    auto user_result = get_user(group_id, chat_id);
    if (!user_result.ok)
        return {false, user_result.message, nullptr};

    auto user = std::get<User>(user_result.value);
    user.is_god = false;

    auto update_result = add_user(group_id, user);
    if (!update_result.ok) {
        LOG(__FUNCTION__, ": Failed to demote god(", chat_id, "): ", update_result.message);
        return {false, "Не удалось лишить пользователя " + user.username + " статуса бога: " + update_result.message};
    }

    return {true, "Пользователь " + user.username + " был лишён статуса бога", nullptr};
}
