#include "Bot.hpp"

#include "Command.hpp"
#include "DBUtils.hpp"
#include "Utils.hpp"

std::shared_ptr<TgBot::Bot> birja::global_bot;

void birja::bot_start(const std::string& token) {
    global_bot = std::make_shared<TgBot::Bot>(token);

    auto me = global_bot->getApi().getMe();
    std::cout << "Starting at " << me->firstName << " / https://t.me/" << me->username << std::endl;

    global_bot->getEvents().onAnyMessage([] (TgBot::Message::Ptr message) {
        auto reply_p = std::make_shared<TgBot::ReplyParameters>();
        reply_p->messageId = message->messageId;
        reply_p->chatId = message->chat->id;

        if (message->chat->type == TgBot::Chat::Type::Private) {
            global_bot->getApi().sendMessage(message->chat->id, "Я *не* работаю в лс, добавь меня в группу", nullptr, reply_p);
        } else {
            if (db::check_user(message->chat->id, message->from->id)) {
                auto get_user_result = db::get_user(message->chat->id, message->from->id);
                if (!get_user_result.ok) {
                    global_bot->getApi().sendMessage(message->chat->id, get_user_result.message, nullptr, reply_p);
                    return;
                }

                auto user = std::get<User>(get_user_result.value);
                if (user.chat_id == message->from->id && user.username != message->from->username) {
                    LOG(__FUNCTION__, ": Updating user: \"", user.username + "\" ==> \"", message->from->username + "\"");
                    auto update_user_result = db::add_user(message->chat->id, {message->from->id, message->from->username, user.balance, user.is_god, message->chat->id});
                    if (!update_user_result.ok) {
                        global_bot->getApi().sendMessage(message->chat->id, update_user_result.message, nullptr, reply_p);
                        return;
                    }
                } else {
                    command::prepare_message(message->text, message);
                    return;
                }
            } else {
            auto add_result = db::add_user(message->chat->id, {message->from->id, message->from->username, 0, false, message->chat->id});
            if (!add_result.ok) {
                    LOG(__FUNCTION__, ": Failed to register the user(", message->from->id, "): ", add_result.message);
                } else
                    LOG(__FUNCTION__, ": The user(", message->from->id, ") has been regisered/updated");
            }

            command::prepare_message(message->text, message);
        }
    });

    global_bot->getEvents().onMyChatMember([] (TgBot::ChatMemberUpdated::Ptr update) {
        if (update->oldChatMember->status == "left" && update->newChatMember->status == "member") {
            LOG(__FUNCTION__, ": The bot has been joined to the group ", update->chat->id);
            global_bot->getApi().sendMessage(update->chat->id, "Да.");

            if (!db::check_user(update->chat->id, update->from->id)) {
                auto add_result = db::add_user(update->chat->id, {update->from->id, update->from->username, 0, true, update->chat->id});
                if (!add_result.ok) {
                    global_bot->getApi().sendMessage(update->chat->id, add_result.message);
                }

                LOG(__FUNCTION__, ": Mark the user ", update->from->id, " as god");
            } else
                LOG(__FUNCTION__, ": The user ", update->from->id, " didn't marked as god(already exists in group)");
        } else
            LOG(__FUNCTION__, ": The bot has left from the group ", update->chat->id);
    });

    try {
        TgBot::TgLongPoll lpoll(*global_bot);
        while (true)
            lpoll.start();
    } catch (TgBot::TgException& tgerr) {
        LOG(__FUNCTION__, ": TgBot error: ", tgerr.what());
    }
}
