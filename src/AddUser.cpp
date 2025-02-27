#include "DBUtils.hpp"

#include <filesystem>

#include <rocksdb/db.h>
#include <boost/json.hpp>

#include "Utils.hpp"

birja::Result birja::db::add_user(std::int64_t group_id, birja::User user) {
    rocksdb::DB* db;
    rocksdb::Options opt;
    opt.create_if_missing = false;

    auto open_result = rocksdb::DB::Open(opt, std::filesystem::path(DBPATH) / std::to_string(group_id), &db);
    if (!open_result.ok()) {
        LOG(__FUNCTION__, ": Failed to open db at ", std::filesystem::path(DBPATH) / std::to_string(group_id), ": ", open_result.ToString());
        return {false, "Не удалось подключиться к базе данных: " + open_result.ToString(), nullptr};
    }

    auto write_result = db->Put(rocksdb::WriteOptions(), std::to_string(user.chat_id), user.to_json());
    if (!write_result.ok()) {
        LOG(__FUNCTION__, ": Failed to add/update user(", user.chat_id, "): ", write_result.ToString());
        return {false, "Не удалось добавить пользователя " + user.username + ": " + write_result.ToString(), nullptr};
    }

    return {true, "Пользователь " + user.username + "успешно добавлен", nullptr};
}
