#include "Command.hpp"

#include <sstream>

#include "DBUtils.hpp"
#include "Utils.hpp"

void birja::command::transfer_currency(icu::UnicodeString& command, TgBot::Message::Ptr message) {
    auto reply_p = std::make_shared<TgBot::ReplyParameters>();
    reply_p->messageId = message->messageId;
    reply_p->chatId = message->chat->id;


    bool is_god = false; /* Is the user has a god(?) */
    auto gods_result = db::get_gods(message->chat->id);
    if (!gods_result.ok) {
        LOG(__FUNCTION__, ": Failed to preparing command: gods_result.ok != true");
        global_bot->getApi().sendMessage(message->chat->id, gods_result.message, nullptr, reply_p);
        return;
    }

    auto gods = std::get<std::vector<std::int64_t>>(gods_result.value);
    if (std::find(gods.begin(), gods.end(), message->from->id) != gods.end())
        is_god = true;

    if (message->replyToMessage) {
        std::string stdstr;
        command.tempSubString(16).toUTF8String(stdstr);
        std::istringstream stream(stdstr);
        std::int64_t quantity = 0;

        stream >> quantity;
        if (quantity == 0)
            return;

        if (!is_god) {
            auto get_sender_result = db::get_user(message->chat->id, message->from->id);
            if (!get_sender_result.ok) {
                global_bot->getApi().sendMessage(message->chat->id, get_sender_result.message, nullptr, reply_p);
                return;
            }

            auto sender = std::get<User>(get_sender_result.value);
            if (sender.balance < quantity) {
                global_bot->getApi().sendMessage(message->chat->id, "Недостаточно валюты:\nТребуется: " + std::to_string(quantity) + "\nВ наличии: " + std::to_string(sender.balance), nullptr, reply_p);
                return;
            }

            sender.balance -= quantity;
            auto update_sender_result = db::add_user(message->chat->id, sender);
            if (!update_sender_result.ok) {
                global_bot->getApi().sendMessage(message->chat->id, update_sender_result.message, nullptr, reply_p);
                return;
            }
        }

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

        global_bot->getApi().sendMessage(message->chat->id, "Вы передали " + (message->replyToMessage->from->username.empty() ? message->replyToMessage->from->firstName : "@" + message->replyToMessage->from->username) + " " + std::to_string(quantity) + " валюты", nullptr, reply_p);
    } else {
        std::string stdstr;
        command.tempSubString(16).toUTF8String(stdstr);
        std::istringstream stream(stdstr);
        std::int64_t quantity = 0;
        std::string username;

        stream >> quantity;
        if (quantity == 0)
            return;
        stream >> username;

        if (!is_god) {
            auto get_sender_result = db::get_user(message->chat->id, message->from->id);
            if (!get_sender_result.ok) {
                global_bot->getApi().sendMessage(message->chat->id, get_sender_result.message, nullptr, reply_p);
                return;
            }

            auto sender = std::get<User>(get_sender_result.value);
            if (sender.balance < quantity) {
                global_bot->getApi().sendMessage(message->chat->id, "Недостаточно валюты:\nТребуется: " + std::to_string(quantity) + "\nВ наличии: " + std::to_string(sender.balance), nullptr, reply_p);
                return;
            }

            sender.balance -= quantity;
            auto update_sender_result = db::add_user(message->chat->id, sender);
            if (!update_sender_result.ok) {
                global_bot->getApi().sendMessage(message->chat->id, update_sender_result.message, nullptr, reply_p);
                return;
            }
        }

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

        global_bot->getApi().sendMessage(message->chat->id, "Вы передали " + (username.starts_with("@") ? username : "@" + username) + " " + std::to_string(quantity) + " валюты", nullptr, reply_p);
    }
}
