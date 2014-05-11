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

#include "../core_module/core_module.hpp"
#include "util_module.hpp"


namespace rpgss {
    namespace script {
        namespace util_module {

            //---------------------------------------------------------
            int util_compress(lua_State* L)
            {
                core::ByteArray* data = core_module::ByteArrayWrapper::Get(L, 1);
                if (data->getSize() == 0) {
                    return luaL_error(L, "empty data");
                }
                io::MemoryFile::Ptr output_stream = io::MemoryFile::New();
                if (util::Compress(data->getBuffer(), data->getSize(), output_stream)) {
                    core::ByteArray::Ptr compressed_data = core::ByteArray::New(output_stream->getBuffer(), output_stream->getSize());
                    core_module::ByteArrayWrapper::Push(L, compressed_data);
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            int util_decompress(lua_State* L)
            {
                core::ByteArray* compressed_data = core_module::ByteArrayWrapper::Get(L, 1);
                if (compressed_data->getSize() == 0) {
                    return luaL_error(L, "empty compressed data");
                }
                io::MemoryFile::Ptr output_stream = io::MemoryFile::New();
                if (util::Decompress(compressed_data->getBuffer(), compressed_data->getSize(), output_stream)) {
                    core::ByteArray::Ptr data = core::ByteArray::New(output_stream->getBuffer(), output_stream->getSize());
                    core_module::ByteArrayWrapper::Push(L, data);
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterUtilModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("util")

                        .addCFunction("compress",   &util_compress)
                        .addCFunction("decompress", &util_decompress)

                    .endNamespace();

                return true;
            }

        } // util_module
    } // namespace script
} // namespace rpgss
