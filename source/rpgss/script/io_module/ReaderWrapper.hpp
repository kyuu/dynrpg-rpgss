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

#ifndef RPGSS_SCRIPT_IO_MODULE_READERWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_IO_MODULE_READERWRAPPER_HPP_INCLUDED

#include "../../io/Reader.hpp"
#include "../lua_include.hpp"
#include "FileWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            class ReaderWrapper {
            public:
                static void Push(lua_State* L, io::Reader* reader);
                static bool Is(lua_State* L, int index);
                static io::Reader* Get(lua_State* L, int index);
                static io::Reader* GetOpt(lua_State* L, int index);

                explicit ReaderWrapper(io::Reader* ptr);

                FileWrapper get_inputStream() const;
                int skipBytes(lua_State* L);
                int readByte(lua_State* L);
                int readBytes(lua_State* L);
                int readString(lua_State* L);
                int readBool(lua_State* L);
                int readInt8(lua_State* L);
                int readInt16(lua_State* L);
                int readInt16BE(lua_State* L);
                int readInt32(lua_State* L);
                int readInt32BE(lua_State* L);
                int readUint8(lua_State* L);
                int readUint16(lua_State* L);
                int readUint16BE(lua_State* L);
                int readUint32(lua_State* L);
                int readUint32BE(lua_State* L);
                int readFloat(lua_State* L);
                int readFloatBE(lua_State* L);
                int readDouble(lua_State* L);
                int readDoubleBE(lua_State* L);

            private:
                io::Reader::Ptr This;
            };

        } // io_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_IO_MODULE_READERWRAPPER_HPP_INCLUDED
