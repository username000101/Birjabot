#include "DBUtils.hpp"

#include <filesystem>

#include <lmdb.h>

birja::Result birja::db::get_user(std::int64_t group_id, std::int64_t chat_id) {
    std::filesystem::path dbdir = std::filesystem::path(DBPATH) / std::to_string(group_id);
    if (!std::filesystem::exists(dbdir) || !std::filesystem::is_directory(dbdir))
        std::filesystem::create_directories(dbdir);

    MDB_env* env;
    MDB_dbi db;
    MDB_txn* tr;

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
    if (int rc = mdb_txn_begin(env, nullptr, MDB_RDONLY, &tr); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось начать транзакцию(получение пользователя): " + std::string(mdb_strerror(rc)), nullptr};
    }

    std::string chat_id_str = std::to_string(chat_id);
    MDB_val result, key;
    key.mv_size = chat_id_str.size();
    key.mv_data = chat_id_str.data();
    if (auto rc = mdb_get(tr, db, &key, &result) != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось выполнить операцию чтения: " + std::string(mdb_strerror(rc)), nullptr};
    }

    if (int rc = mdb_txn_commit(tr); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось закомментировать изменения(получение пользователя): " + std::string(mdb_strerror(rc)), nullptr};
    }

    return {true, "", User::from_json(std::string(static_cast<const char*>(result.mv_data), result.mv_size))};
}
