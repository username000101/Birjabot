#include "DBUtils.hpp"

bool birja::db::check_user(std::int64_t group_id, std::int64_t chat_id) {
    return get_user(group_id, chat_id).ok;
}
