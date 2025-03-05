#include "DBUtils.hpp"

#include <filesystem>
#include <memory>

#include <boost/json.hpp>

birja::Result birja::db::get_gods(std::int64_t group_id) {
    std::filesystem::path dbdir = std::filesystem::path(DBPATH) / std::to_string(group_id);
    if (!std::filesystem::exists(dbdir) || !std::filesystem::is_directory(dbdir))
        std::filesystem::create_directories(dbdir);

    MDB_env* env;
    MDB_dbi db;
    MDB_txn* tr;
    MDB_cursor* cursor;


    if (int rc = mdb_env_create(&env); rc != 0)
        return {false, "Не удалось инициализировать среду: " + std::string(mdb_strerror(rc)), nullptr};
    if (int rc = mdb_env_set_mapsize(env, 104857600 /* 100MB */); rc != 0)
        return {false, "Не удалось установить размер базы данных: " + std::string(mdb_strerror(rc)), nullptr};
    if (int rc = mdb_env_open(env, dbdir.c_str(), 0, 0664); rc != 0)
        return {false, "Не удалось открыть базу данных: " + std::string(mdb_strerror(rc)), nullptr};

    if (int rc = mdb_txn_begin(env, nullptr, 0, &tr); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось начать транзакцию(добавление базы данных в окружение)" + std::string(mdb_strerror(rc)), nullptr};
    }
    if (int rc = mdb_dbi_open(tr, nullptr, 0, &db); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось добавить базу данных в окружение: " + std::string(mdb_strerror(rc)), nullptr};
    }
    if (int rc = mdb_txn_commit(tr); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось закомментировать изменения(добавление базы данных в окружение): " + std::string(mdb_strerror(rc)), nullptr};
    }
    if (int rc = mdb_txn_begin(env, nullptr, 0, &tr); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось начать транзакцию(поиск \"богов\"): " + std::string(mdb_strerror(rc)), nullptr};
    }
    if (int rc = mdb_cursor_open(tr, db, &cursor); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось настроить курсор: " + std::string(mdb_strerror(rc)), nullptr};
    }

    MDB_val key, value;
    std::vector<std::int64_t> result;
    while (mdb_cursor_get(cursor, &key, &value, MDB_NEXT) == 0) {
        auto user = User::from_json(std::string(static_cast<const char*>(value.mv_data), value.mv_size));
        if (user.is_god)
            result.push_back(user.chat_id);
    }

    lmdb_close_instance(db, env);
    return {true, "", result};
}
