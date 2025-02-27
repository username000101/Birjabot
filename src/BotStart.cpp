#include "Bot.hpp"

#include "DBUtils.hpp"
#include "Utils.hpp"

std::shared_ptr<TgBot::Bot> birja::global_bot;

void birja::bot_start(const std::string& token) {
    global_bot = std::make_shared<TgBot::Bot>(token);

    global_bot->getEvents().onAnyMessage([] (TgBot::Message::Ptr message) {
        auto reply_p = std::make_shared<TgBot::ReplyParameters>();
        reply_p->messageId = message->messageId;
        reply_p->chatId = message->chat->id;

        if (message->chat->type == TgBot::Chat::Type::Private) {
            global_bot->getApi().sendMessage(message->chat->id, "Я *не* работаю в лс, добавь меня в группу", nullptr, reply_p);
        } else {
            if (!db::check_user(message->chat->id, message->from->id)) {
                auto add_result = db::add_user(message->chat->id, {false, message->from->username, 0, message->from->id, message->chat->id});
                if (!add_result.ok) {
                    LOG(__FUNCTION__, ": Failed to register the user(", message->from->id, "): ", add_result.message);
                    global_bot->getApi().sendMessage(message->chat->id, add_result.message, nullptr, reply_p);
                } else {
                    LOG(__FUNCTION__, ": The user(", message->from->id, ") has been regisered");
                }
            }
        }
    });

    global_bot->getEvents().onMyChatMember([] (TgBot::ChatMemberUpdated::Ptr update) {
        if (update->oldChatMember->status == "left" && update->newChatMember->status == "member") {
            LOG(__FUNCTION__, ": The bot has been joined to the group ", update->chat->id);
            global_bot->getApi().sendMessage(update->chat->id, "Да.");

            auto add_result = db::add_user(update->chat->id, {true, update->from->username, 0, update->from->id, update->chat->id});
            if (!add_result.ok)
                global_bot->getApi().sendMessage(update->chat->id, add_result.message);
        }
    });

    try {
        TgBot::TgLongPoll lpoll(*global_bot);
        while (true)
            lpoll.start();
    } catch (TgBot::TgException& tgerr) {
        LOG(__FUNCTION__, ": TgBot error: ", tgerr.what());
    }
}
