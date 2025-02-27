#include "DBUtils.hpp"

#include <filesystem>

#include <rocksdb/db.h>

#include "Utils.hpp"

bool birja::db::check_user(std::int64_t group_id, std::int64_t chat_id) {
    rocksdb::DB* db;
    rocksdb::Options opt;
    opt.create_if_missing = false;

    auto open_result = rocksdb::DB::Open(opt, std::filesystem::path(DBPATH) / std::to_string(group_id), &db);
    if (!open_result.ok()) {
        LOG(__FUNCTION__, ": Failed to open db at ", std::filesystem::path(DBPATH) / std::to_string(group_id), ": ", open_result.ToString());
        return false;
    }

    std::string result;
    auto read_result = db->Get(rocksdb::ReadOptions(), std::to_string(chat_id), &result);
    if (read_result.IsNotFound())
        return false;
    else if (!read_result.ok()) {
        LOG(__FUNCTION__, ": Failed to check user(", chat_id, "): ", read_result.ToString());
        return false;
    } else
        return true;
}
