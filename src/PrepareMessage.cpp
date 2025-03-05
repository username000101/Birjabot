#include "Command.hpp"

#include <clocale>

#include <unicode/unistr.h>

void birja::command::prepare_message(const std::string& command, TgBot::Message::Ptr message) {
    auto lcommand = icu::UnicodeString::fromUTF8(command.c_str()).toLower();

    if (lcommand.startsWith("открыть кошелек") || lcommand.startsWith("открыть кошелёк"))
        open_wallet(lcommand, message);
    else if (lcommand.startsWith("передать валюту"))
        transfer_currency(lcommand, message);
    else if (lcommand.startsWith("обнулить кошелек") || lcommand.startsWith("обнулить кошелёк"))
        reset_wallet(lcommand, message);
    else if (lcommand.startsWith("пополнить баланс"))
        give_currency(lcommand, message);
    else if (lcommand.startsWith("изъять") || lcommand.startsWith("изьять"))
        generic_handler(lcommand, message);
    else if (lcommand.startsWith("дать кошелек бога") || lcommand.startsWith("дать кошелёк бога"))
        add_god(lcommand, message);
}
