#include "DBUtils.hpp"

#include <filesystem>

#include <rocksdb/db.h>
#include <boost/json.hpp>

#include "Utils.hpp"

birja::Result birja::db::add_god(std::int64_t group_id, std::int64_t chat_id) {
    rocksdb::DB* db;
    rocksdb::Options opt;
    opt.create_if_missing = false;

    auto open_result = rocksdb::DB::Open(opt, std::filesystem::path(DBPATH) / std::to_string(group_id), &db);
    if (!open_result.ok()) {
        LOG(__FUNCTION__, ": Failed to open db at ", std::filesystem::path(DBPATH) / std::to_string(group_id), ": ", open_result.ToString());
        return {false, "Не удалось подключиться к базе данных: " + open_result.ToString(), nullptr};
    }

    auto user_result = get_user(group_id, chat_id);
    if (!user_result.ok)
        return {false, user_result.message, nullptr};

    auto user = std::get<User>(user_result.value);
    user.is_god = true;

    auto update_result = add_user(group_id, user);
    if (!update_result.ok) {
        LOG(__FUNCTION__, ": Failed to add god(", chat_id, "): ", update_result.message);
        return {false, update_result.message, nullptr};
    }

    return {true, "Пользователь " + convert_chatid_to_username(group_id, chat_id) + " получил статус бога", nullptr};
}
