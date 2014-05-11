/*
    The MIT License (MIT)

    Copyright (c) 2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "../../Context.hpp"
#include "core_module.hpp"


namespace rpgss {
    namespace script {
        namespace core_module {

            //---------------------------------------------------------
            int core_newByteArray(lua_State* L)
            {
                int nargs = lua_gettop(L);

                if (nargs == 0) {
                    // newByteArray()
                    ByteArrayWrapper::Push(L, core::ByteArray::New());
                } else {
                    if (lua_isnumber(L, 1)) {
                        // newByteArray(size [, value])
                        int size = luaL_checkint(L, 1);
                        int value = luaL_optint(L, 2, 0);
                        if (size < 0) {
                            return luaL_error(L, "invalid size");
                        }
                        ByteArrayWrapper::Push(L, core::ByteArray::New(size, value));
                    } else {
                        // newByteArray(string)
                        size_t len;
                        const char* str = lua_tolstring(L, 1, &len);
                        ByteArrayWrapper::Push(L, core::ByteArray::New((const u8*)str, len));
                    }
                }

                return 1;
            }

            //---------------------------------------------------------
            bool RegisterCoreModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("core")

                        .beginClass<ByteArrayWrapper>("ByteArray")
                            .addProperty("size",                &ByteArrayWrapper::get_size)
                            .addCFunction("__len",              &ByteArrayWrapper::__len)
                            .addCFunction("__add",              &ByteArrayWrapper::__add)
                            .addCFunction("__concat",           &ByteArrayWrapper::__concat)
                            .addCFunction("toString",           &ByteArrayWrapper::toString)
                            .addCFunction("getByte",            &ByteArrayWrapper::getByte)
                            .addCFunction("setByte",            &ByteArrayWrapper::setByte)
                            .addCFunction("resize",             &ByteArrayWrapper::resize)
                            .addCFunction("memset",             &ByteArrayWrapper::memset)
                            .addCFunction("clear",              &ByteArrayWrapper::clear)
                        .endClass()

                        .addCFunction("newByteArray",   &core_newByteArray)

                    .endNamespace();

                return true;
            }

        } // core_module
    } // namespace script
} // namespace rpgss
