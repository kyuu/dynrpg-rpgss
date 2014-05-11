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

#ifndef RPGSS_SCRIPT_IO_MODULE_WRITERWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_IO_MODULE_WRITERWRAPPER_HPP_INCLUDED

#include "../../io/Writer.hpp"
#include "../lua_include.hpp"
#include "FileWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            class WriterWrapper {
            public:
                static void Push(lua_State* L, io::Writer* binarywriter);
                static bool Is(lua_State* L, int index);
                static io::Writer* Get(lua_State* L, int index);
                static io::Writer* GetOpt(lua_State* L, int index);

                explicit WriterWrapper(io::Writer* ptr);

                FileWrapper get_outputStream() const;
                int writeByte(lua_State* L);
                int writeBytes(lua_State* L);
                int writeString(lua_State* L);
                int writeBool(lua_State* L);
                int writeInt8(lua_State* L);
                int writeInt16(lua_State* L);
                int writeInt16BE(lua_State* L);
                int writeInt32(lua_State* L);
                int writeInt32BE(lua_State* L);
                int writeUint8(lua_State* L);
                int writeUint16(lua_State* L);
                int writeUint16BE(lua_State* L);
                int writeUint32(lua_State* L);
                int writeUint32BE(lua_State* L);
                int writeFloat(lua_State* L);
                int writeFloatBE(lua_State* L);
                int writeDouble(lua_State* L);
                int writeDoubleBE(lua_State* L);

            private:
                io::Writer::Ptr This;
            };

        } // io_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_IO_MODULE_WRITERWRAPPER_HPP_INCLUDED
