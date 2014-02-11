#include "Log.hpp"
#include "Guard.hpp"


namespace rpgss {
    namespace debug {

        //---------------------------------------------------------
        Guard::Guard(const std::string& blockName)
            : _blockName(blockName)
        {
            Log() << "+ " << _blockName;
            Log::IncrementIndent();
        }

        //---------------------------------------------------------
        Guard::~Guard() {
            Log::DecrementIndent();
            Log() << "- " << _blockName;
        }

    } // namespace debug
} // namespace rpgss
