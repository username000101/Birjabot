#include "Command.hpp"

#include <boost/locale.hpp>

#include "DBUtils.hpp"
#include "Utils.hpp"

void birja::command::give_currency(icu::UnicodeString& command, TgBot::Message::Ptr message) {
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
        LOG(__FUNCTION__, ": Attempt to give the currency: unathorized");
        global_bot->getApi().sendMessage(message->chat->id, "Вы не обладаете правами для выполнения данной команды", nullptr, reply_p);
        return;
    }

    if (message->replyToMessage) {
        std::string stream_command;
        command.tempSubString(17).toUTF8String(stream_command);
        std::istringstream stream(stream_command);
        std::string tmp;
        std::int64_t quantity = 0;

        stream >> tmp;
        stream >> quantity;

        if (quantity == 0)
            return;

        LOG(__FUNCTION__, ": Attempt to give ", quantity, " tokens ", message->replyToMessage->from->username);

        auto get_user_result = db::get_user(message->chat->id, message->replyToMessage->from->id);
        if (!get_user_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, get_user_result.message, nullptr, reply_p);
            return;
        }

        auto user = std::get<User>(get_user_result.value);
        user.balance += quantity;

        auto update_user_result = db::add_user(message->chat->id, user);
        if (!update_user_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, update_user_result.message, nullptr, reply_p);
            return;
        }

        global_bot->getApi().sendMessage(message->chat->id, "Пополнил счёт " + (message->replyToMessage->from->username.empty() ? message->replyToMessage->from->firstName : "@" + message->replyToMessage->from->username) + " на " + std::to_string(quantity), nullptr, reply_p);
        return;
    }

    std::string stream_command;
    command.tempSubString(17).toUTF8String(stream_command);
    std::istringstream stream(stream_command);
    std::string tmp;
    std::string username;
    std::int64_t quantity;

    stream >> username;
    stream >> tmp;
    stream >> quantity;

    LOG(__FUNCTION__, ": Attempt to give ", quantity, " tokens ", username);
    auto get_user_result = db::get_user(message->chat->id, username);
    if (!get_user_result.ok) {
        global_bot->getApi().sendMessage(message->chat->id, get_user_result.message, nullptr, reply_p);
        return;
    }

    auto user = std::get<User>(get_user_result.value);
    user.balance += quantity;

    auto update_user_result = db::add_user(message->chat->id, user);
    if (!update_user_result.ok) {
        global_bot->getApi().sendMessage(message->chat->id, update_user_result.message, nullptr, reply_p);
        return;
    }

    global_bot->getApi().sendMessage(message->chat->id, "Пополнил счёт " + (username.starts_with("@") ? username : "@" + username) + " на " + std::to_string(quantity), nullptr, reply_p);

}
