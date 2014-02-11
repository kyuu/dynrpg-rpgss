#include <string>

#include "common/types.hpp"


namespace rpgss {

    class TokenParser {
    public:
        enum {
            ParsedUndefined,
            ParsedBoolean,
            ParsedNumber,
            ParsedString,
            ParsedColor,
        } ParsedType;

        bool Boolean;
        double Number;
        std::string String;
        u32 Color;

        TokenParser() : ParsedType(ParsedUndefined) {}

        bool parse(const char* token);

    private:
        enum {
            Start,
            ParsingVariable,
            ParsingSwitch,
            ParsingColor,
            ParsingActorName,
        } State;
    };

} // namespace rpgss
