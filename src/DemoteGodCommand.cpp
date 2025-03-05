#include "Command.hpp"

#include "DBUtils.hpp"
#include "Utils.hpp"

void birja::command::demote_god(icu::UnicodeString& command, TgBot::Message::Ptr message) {
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
        LOG(__FUNCTION__, ": Attempt to unmark the user: unathorized");
        global_bot->getApi().sendMessage(message->chat->id, "Вы не обладаете правами для выполнения данной команды", nullptr, reply_p);
        return;
    }

    if (message->replyToMessage) {
        global_bot->getApi().sendMessage(message->chat->id, db::demote_god(message->chat->id, message->replyToMessage->from->id).message, nullptr, reply_p);
        return;
    }

    std::string stream_command;
    command.tempSubString(12).toUTF8String(stream_command);
    std::istringstream stream(stream_command);
    std::string username;
    stream >> username;

    LOG(__FUNCTION__, ": Attempt to unmark the ", username, " as god");
    global_bot->getApi().sendMessage(message->chat->id, db::demote_god(message->chat->id, username).message, nullptr, reply_p);
}
