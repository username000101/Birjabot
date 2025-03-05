#include "Command.hpp"

#include <sstream>

#include "Utils.hpp"

void birja::command::generic_handler(icu::UnicodeString& command, TgBot::Message::Ptr message) {
    std::string stdstring;
    command.tempSubString(7).toUTF8String(stdstring);

    std::istringstream stream(stdstring);
    std::string subcommand;

    stream >> subcommand;
    if (icu::UnicodeString::fromUTF8(subcommand).toLower() == "душу") {
        LOG(__FUNCTION__, ": Attempt to call the 'demote_god' command");
        demote_god(command, message);
    } else {
        LOG(__FUNCTION__, ": Attempt to call the 'seize_user_balance' command");
        seize_user_balance(command, message);
    }
}
