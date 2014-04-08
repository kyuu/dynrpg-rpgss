#ifndef RPGSS_SCRIPT_CORE_MODULE_BYTEARRAYWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_CORE_MODULE_BYTEARRAYWRAPPER_HPP_INCLUDED

#include "../../core/ByteArray.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace core_module {

            class ByteArrayWrapper {
            public:
                static void Push(lua_State* L, core::ByteArray* bytearray);
                static bool Is(lua_State* L, int index);
                static core::ByteArray* Get(lua_State* L, int index);
                static core::ByteArray* GetOpt(lua_State* L, int index);

                explicit ByteArrayWrapper(core::ByteArray* ptr);

                int get_size() const;
                int __len(lua_State* L);
                int __add(lua_State* L);
                int __concat(lua_State* L);
                int toString(lua_State* L);
                int getByte(lua_State* L);
                int setByte(lua_State* L);
                int resize(lua_State* L);
                int memset(lua_State* L);
                int clear(lua_State* L);

            private:
                core::ByteArray::Ptr This;
            };

        } // core_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_CORE_MODULE_BYTEARRAYWRAPPER_HPP_INCLUDED
