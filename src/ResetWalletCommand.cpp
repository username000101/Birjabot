#include "Command.hpp"

#include <boost/locale.hpp>

#include "DBUtils.hpp"
#include "Utils.hpp"

void birja::command::reset_wallet(icu::UnicodeString& command, TgBot::Message::Ptr message) {
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
        LOG(__FUNCTION__, ": Attempt to reset the user balance: unathorized");
        global_bot->getApi().sendMessage(message->chat->id, "Вы не обладаете правами для выполнения данной команды", nullptr, reply_p);
        return;
    }

    if (message->replyToMessage) {
        auto reset_wallet_result = db::set_user_balance(message->chat->id, message->replyToMessage->from->id, 0);
        if (!reset_wallet_result.ok) {
            global_bot->getApi().sendMessage(message->chat->id, reset_wallet_result.message, nullptr, reply_p);
            return;
        }

        global_bot->getApi().sendMessage(message->chat->id, "Обнулил счёт " + (message->replyToMessage->from->username.empty() ? message->replyToMessage->from->firstName : "@" + message->replyToMessage->from->username), nullptr, reply_p);
        return;
    }

    std::string stream_command;
    command.tempSubString(17).toUTF8String(stream_command);
    std::istringstream stream(stream_command);
    std::string username;
    stream >> username;

    auto reset_wallet_result = db::set_user_balance(message->chat->id, username, 0);
    if (!reset_wallet_result.ok) {
        global_bot->getApi().sendMessage(message->chat->id, reset_wallet_result.message, nullptr, reply_p);
        return;
    }

    global_bot->getApi().sendMessage(message->chat->id, "Обнулил счёт " + (username.starts_with("@") ? username : "@" + username), nullptr, reply_p);

}
