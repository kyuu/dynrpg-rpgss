#ifndef RPGSS_SCRIPT_IO_MODULE_FILEWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_IO_MODULE_FILEWRAPPER_HPP_INCLUDED

#include "../../io/File.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            class FileWrapper {
            public:
                static void Push(lua_State* L, io::File* file);
                static bool Is(lua_State* L, int index);
                static io::File* Get(lua_State* L, int index);
                static io::File* GetOpt(lua_State* L, int index);

                explicit FileWrapper(io::File* ptr);

                bool get_isOpen() const;
                int get_size() const;
                int __len(lua_State* L);
                int close(lua_State* L);
                bool get_eof() const;
                bool get_error() const;
                bool get_good() const;
                int clearError(lua_State* L);
                int seek(lua_State* L);
                int tell(lua_State* L);
                int read(lua_State* L);
                int write(lua_State* L);
                int flush(lua_State* L);

            private:
                io::File::Ptr This;
            };

        } // io_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_IO_MODULE_FILEWRAPPER_HPP_INCLUDED
