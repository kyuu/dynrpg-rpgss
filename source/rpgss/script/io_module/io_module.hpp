#ifndef RPGSS_SCRIPT_IO_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_IO_MODULE_HPP_INCLUDED

#include "../../io/io.hpp"
#include "../lua_include.hpp"
#include "FileWrapper.hpp"
#include "MemoryFileWrapper.hpp"
#include "ReaderWrapper.hpp"
#include "WriterWrapper.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            bool RegisterIoModule(lua_State* L);

        } // io_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_IO_MODULE_HPP_INCLUDED
