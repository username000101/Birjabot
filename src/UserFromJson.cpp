#include "Types.hpp"

#include <boost/json.hpp>

#include "Utils.hpp"

birja::User birja::User::from_json(const std::string& json) {
    if (json.empty())
        return {};

    try {
        auto object = boost::json::parse(json).at("user").as_object();
        std::string username;
        std::int64_t group_id, chat_id, balance;
        bool is_god;

        if (!object.contains("chat_id")) {
            LOG(__FUNCTION__, ": Failed to parse json: not found the 'chat_id' field");
            return {};
        }
        chat_id = std::stoll((const std::string)object.at("chat_id").as_string());

        if (!object.contains("username"))
            LOG(__FUNCTION__, ": Warning: not found the 'username' field");
        else
            username = object.at("username").as_string();

        if (!object.contains("balance")) {
            LOG(__FUNCTION__, ": Failed to parse json: not found the 'balance' field");
            return {};
        }
        balance = std::stoll((const std::string)object.at("balance").as_string());

        if (!object.contains("is_god")) {
            LOG(__FUNCTION__, ": Failed to parse json: not found the 'is_god' field");
            return {};
        }
        is_god = object.at("is_god").as_bool();

        if (!object.contains("group_id"))
            LOG(__FUNCTION__, ": Warning: not found the 'group_id' field");
        else
            group_id = std::stoll((const std::string)object.at("group_id").as_string());

        return {chat_id, username, balance, is_god, group_id};
    } catch (boost::system::system_error& jsonerr) {
        LOG(__FUNCTION__, ": Failed to parse json: boost::system::system_error: ", jsonerr.what());
        return {};
    }
}
