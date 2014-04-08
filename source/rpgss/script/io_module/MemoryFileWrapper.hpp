#ifndef RPGSS_SCRIPT_IO_MODULE_MEMORYFILEWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_IO_MODULE_MEMORYFILEWRAPPER_HPP_INCLUDED

#include "../../io/MemoryFile.hpp"
#include "../lua_include.hpp"
#include "FileWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            class MemoryFileWrapper : public FileWrapper {
            public:
                static void Push(lua_State* L, io::MemoryFile* memoryfile);
                static bool Is(lua_State* L, int index);
                static io::MemoryFile* Get(lua_State* L, int index);
                static io::MemoryFile* GetOpt(lua_State* L, int index);

                explicit MemoryFileWrapper(io::MemoryFile* ptr);

                bool get_size() const;
                bool get_capacity() const;
                int __len(lua_State* L);
                int copyBuffer(lua_State* L);
                int reserve(lua_State* L);
                int clear(lua_State* L);

            private:
                io::MemoryFile::Ptr This;
            };

        } // io_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_IO_MODULE_MEMORYFILEWRAPPER_HPP_INCLUDED
