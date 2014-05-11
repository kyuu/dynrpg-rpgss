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
