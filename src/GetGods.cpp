#include "DBUtils.hpp"

#include <filesystem>
#include <memory>

#include <rocksdb/db.h>
#include <boost/json.hpp>

#include "Utils.hpp"

birja::Result birja::db::get_gods(std::int64_t group_id) {
    rocksdb::DB* db;
    rocksdb::Options opt;
    opt.create_if_missing = false;

    auto open_result = rocksdb::DB::Open(opt, std::filesystem::path(DBPATH) / std::to_string(group_id), &db);
    if (!open_result.ok()) {
        LOG(__FUNCTION__, ": Failed to open db at ", std::filesystem::path(DBPATH) / std::to_string(group_id), ": ", open_result.ToString());
        return {false, "Не удалось подключиться к базе данных: " + open_result.ToString(), nullptr};
    }

    std::unique_ptr<rocksdb::Iterator> iter(db->NewIterator(rocksdb::ReadOptions()));
    std::vector<std::int64_t> result;
    for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
        auto json = boost::json::parse(iter->value().ToString()).at("user").as_object();
        if (!json.contains("is_god")) {
            LOG("Invalid JSON: ", iter->key().ToString(), " don't contains the 'is_god' value");
            return {false, "Неправильный JSON: объект user не содержит значения 'is_god'", nullptr};
        } else if (json.at("is_god").as_bool())
            result.push_back(std::stoll(iter->key().ToString()));
    }

    return {true, "", result};
}
