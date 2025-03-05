#include "Command.hpp"

#include <sstream>

#include "DBUtils.hpp"
#include "Utils.hpp"

void birja::command::seize_user_balance(icu::UnicodeString& command, TgBot::Message::Ptr message) {
    auto reply_p = std::make_shared<TgBot::ReplyParameters>();
    reply_p->messageId = message->messageId;
    reply_p->chatId = message->chat->id;

    auto gods_result = db::get_gods(message->chat->id);
    if (!gods_result.ok) {
        LOG(__FUNCTION__, ": Failed to preparing command: gods_result.ok != true");
        global_bot->getApi().sendMessage(message->chat->id, gods_result.message, nullptr, reply_p);
        return;
    }

    auto gods = std::get<std::vector<std::int64_t>>(gods_result.value);
    if (std::find(gods.begin(), gods.end(), message->from->id) == gods.end()) {
        LOG(__FUNCTION__, ": Attempt to seize the user balance: unathorized");
        global_bot->getApi().sendMessage(message->chat->id, "Вы не обладаете правами для выполнения данной команды", nullptr, reply_p);
        return;
    }

    if (message->replyToMessage) {
        std::string stdstring;
        command.tempSubString(7).toUTF8String(stdstring);
        std::istringstream stream(stdstring);
        std::int64_t quantity = 0;

        stream >> quantity;
        if (quantity == 0)
            return;

        auto get_user_result = db::get_user(message->chat->id, message->replyToMessage->from->id);
        if (!get_user_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, get_user_result.message, nullptr, reply_p);
            return;
        }

        auto user = std::get<User>(get_user_result.value);
        user.balance -= quantity;

        auto update_user_result = db::add_user(message->chat->id, user);
        if (!update_user_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, update_user_result.message, nullptr, reply_p);
            return;
        }

        global_bot->getApi().sendMessage(message->chat->id, "Изъял " + std::to_string(quantity) + " из кошелька " + (message->replyToMessage->from->username.empty() ? message->replyToMessage->from->firstName : "@" + message->replyToMessage->from->username), nullptr, reply_p);
        return;
    } else {
        std::string stdstring;
        command.tempSubString(7).toUTF8String(stdstring);
        std::istringstream stream(stdstring);
        std::string tmp, username;
        std::int64_t quantity = 0;

        stream >> quantity;
        if (quantity == 0)
            return;

        stream >> tmp >> tmp;
        stream >> username;

        auto get_user_result = db::get_user(message->chat->id, username);
        if (!get_user_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, get_user_result.message, nullptr, reply_p);
            return;
        }

        auto user = std::get<User>(get_user_result.value);
        user.balance -= quantity;

        auto update_user_result = db::add_user(message->chat->id, user);
        if (!update_user_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, update_user_result.message, nullptr, reply_p);
            return;
        }

        global_bot->getApi().sendMessage(message->chat->id, "Изъял " + std::to_string(quantity) + " из кошелька " + (username.starts_with("@") ? username : "@" + username), nullptr, reply_p);
    }
}
