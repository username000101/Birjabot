#pragma once

#include <memory>

#include <unicode/unistr.h>
#include <tgbot/tgbot.h>

namespace birja {
    extern std::shared_ptr<TgBot::Bot> global_bot;

    namespace command {
        void add_god(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void demote_god(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void open_wallet(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void reset_wallet(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void give_currency(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void transfer_currency(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void seize_user_balance(icu::UnicodeString& command, TgBot::Message::Ptr message);
        void topup_user_balance(icu::UnicodeString& command, TgBot::Message::Ptr message);

        void generic_handler(icu::UnicodeString& command, TgBot::Message::Ptr message); // For "изъять душу" and "изъять [num]"
        void prepare_message(const std::string& command, TgBot::Message::Ptr message);
    }
}
