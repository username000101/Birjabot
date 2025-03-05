#include "DBUtils.hpp"

#include <algorithm>
#include <filesystem>

#include <lmdb.h>

birja::Result birja::db::add_god(std::int64_t group_id, std::int64_t chat_id) {
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
    if (int rc = mdb_txn_begin(env, nullptr, 0, &tr); rc != 0) {
        lmdb_close_instance(db, env);
        return {false, "Не удалось начать транзакцию(добавление бога): " + std::string(mdb_strerror(rc)), nullptr};
    }

    auto gods = db::get_gods(group_id);
    if (!gods.ok) {
        lmdb_close_instance(db, env);
        return {false, gods.message, nullptr};
    }

    auto gods_v = std::get<std::vector<std::int64_t>>(gods.value);
    if (std::find(gods_v.begin(), gods_v.end(), chat_id) != gods_v.end()) {
        lmdb_close_instance(db, env);
        return {true, "Пользователь " + convert_chatid_to_username(group_id, chat_id) + " уже имеет статус бога", nullptr};
    }
    auto get_user_result = get_user(group_id, chat_id);
    if (!get_user_result.ok) {
        lmdb_close_instance(db, env);
        return {false, get_user_result.message, nullptr};
    }
    auto user = std::get<User>(get_user_result.value);
    user.is_god = true;

    auto update_user_result = add_user(group_id, user);
    if (!update_user_result.ok) {
        lmdb_close_instance(db, env);
        return {false, update_user_result.message, nullptr};
    }

    lmdb_close_instance(db, env);
    return {true, "Пользователь " + convert_chatid_to_username(group_id, chat_id) + " получил статус бога", nullptr};
}
