#pragma once

#include <memory>

#include <tgbot/tgbot.h>

namespace birja {
    extern std::shared_ptr<TgBot::Bot> global_bot;
    void bot_start(const std::string& token = TOKEN);
}
