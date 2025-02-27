#pragma once

#include <string>


#include <boost/locale.hpp>
#include <tgbot/tgbot.h>

namespace birja {
    namespace command {
        void add_god(const std::string& command, TgBot::Message::Ptr message);
        void demote_god(const std::string& command, TgBot::Message::Ptr message);
        void open_wallet(const std::string& command, TgBot::Message::Ptr message);
        void reset_wallet(const std::string& command, TgBot::Message::Ptr message);
        void seize_user_balance(const std::string& command, TgBot::Message::Ptr message);
        void topup_user_balance(const std::string& command, TgBot::Message::Ptr message);

        void prepare_message(const std::string& command, TgBot::Message::Ptr message);
    }
}
