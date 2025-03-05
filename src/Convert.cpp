#include "DBUtils.hpp"

#include <filesystem>

#include <lmdb.h>

std::string birja::db::convert_chatid_to_username(std::int64_t group_id, std::int64_t chat_id) {
    std::filesystem::path dbdir = std::filesystem::path(DBPATH) / std::to_string(group_id);
    if (!std::filesystem::exists(dbdir) || !std::filesystem::is_directory(dbdir))
        std::filesystem::create_directories(dbdir);

    MDB_env* env;
    MDB_dbi db;
    MDB_txn* tr;
    MDB_cursor* cursor;


    if (mdb_env_create(&env) != 0)
        return "";
    if (mdb_env_set_mapsize(env, 104857600 /* 100MB */) != 0)
        return "";
    if (mdb_env_open(env, dbdir.c_str(), 0, 0664) != 0)
        return "";

    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &tr) != 0) {
        lmdb_close_instance(db, env);
        return "";
    }
    if (mdb_dbi_open(tr, nullptr, 0, &db) != 0) {
        lmdb_close_instance(db, env);
        return "";
    }
    if (mdb_txn_commit(tr) != 0) {
        lmdb_close_instance(db, env);
        return "";
    }
    if (mdb_txn_begin(env, nullptr, 0, &tr) != 0) {
        lmdb_close_instance(db, env);
        return "";
    }
    if (mdb_cursor_open(tr, db, &cursor) != 0) {
        lmdb_close_instance(db, env);
        return "";
    }

    MDB_val key, value;
    std::vector<std::int64_t> result;
    while (mdb_cursor_get(cursor, &key, &value, MDB_NEXT) == 0) {
        auto user = User::from_json(std::string(static_cast<const char*>(value.mv_data), value.mv_size));
        if (user.chat_id == chat_id)
            return user.username;
    }

    return "";
}

std::int64_t birja::db::convert_username_to_chatid(std::int64_t group_id, const std::string& username) {
    std::filesystem::path dbdir = std::filesystem::path(DBPATH) / std::to_string(group_id);
    if (!std::filesystem::exists(dbdir) || !std::filesystem::is_directory(dbdir))
        std::filesystem::create_directories(dbdir);

    MDB_env* env;
    MDB_dbi db;
    MDB_txn* tr;
    MDB_cursor* cursor;


    if (mdb_env_create(&env) != 0)
        return 0;
    if (mdb_env_set_mapsize(env, 104857600 /* 100MB */) != 0)
        return 0;
    if (mdb_env_open(env, dbdir.c_str(), 0, 0664) != 0)
        return 0;

    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &tr) != 0) {
        lmdb_close_instance(db, env);
        return 0;
    }
    if (mdb_dbi_open(tr, nullptr, 0, &db) != 0) {
        lmdb_close_instance(db, env);
        return 0;
    }
    if (mdb_txn_commit(tr) != 0) {
        lmdb_close_instance(db, env);
        return 0;
    }
    if (mdb_txn_begin(env, nullptr, 0, &tr) != 0) {
        lmdb_close_instance(db, env);
        return 0;
    }
    if (mdb_cursor_open(tr, db, &cursor) != 0) {
        lmdb_close_instance(db, env);
        return 0;
    }

    MDB_val key, value;
    std::vector<std::int64_t> result;
    while (mdb_cursor_get(cursor, &key, &value, MDB_NEXT) == 0) {
        auto user = User::from_json(std::string(static_cast<const char*>(value.mv_data), value.mv_size));
        if (user.username == (username.starts_with("@") ? username.substr(1) : username))
            return user.chat_id;
    }

    return 0;
}
