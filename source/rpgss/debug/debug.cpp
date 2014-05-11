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

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../core/ByteArray.hpp"
#include "debug.hpp"


namespace rpgss {
    namespace debug {

        //---------------------------------------------------------
        void ShowMessage(const char* format, ...)
        {
            core::ByteArray::Ptr buffer = core::ByteArray::New(4096);
            std::string msg;

            // format message
            va_list arg_list;

        try_again:

            va_start(arg_list, format);

            int size = vsnprintf((char*)buffer->getBuffer(), buffer->getSize(), format, arg_list);

            if (size >= buffer->getSize()) { // buffer was not big enough to hold the output string + terminating null character
                buffer->resize(size + 1);
                goto try_again;
            } else if (size < 0) { // format error occurred, just show the raw format string
                msg = format;
            } else { // ok
                msg = (const char*)buffer->getBuffer();
            }

            va_end(arg_list);

            MessageBox(0, msg.c_str(), "DynRPG:RPGSS", MB_OK | MB_TASKMODAL);
        }

        //---------------------------------------------------------
        void ShowError(const char* format, ...)
        {
            core::ByteArray::Ptr buffer = core::ByteArray::New(4096);
            std::string msg;

            // format message
            va_list arg_list;

        try_again:

            va_start(arg_list, format);

            int size = vsnprintf((char*)buffer->getBuffer(), buffer->getSize(), format, arg_list);

            if (size >= buffer->getSize()) { // buffer was not big enough to hold the output string + terminating null character
                buffer->resize(size + 1);
                goto try_again;
            } else if (size < 0) { // format error occurred, just show the raw format string
                msg = format;
            } else { // ok
                msg = (const char*)buffer->getBuffer();
            }

            va_end(arg_list);

            MessageBox(0, msg.c_str(), "DynRPG:RPGSS", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        }

    } // namespace debug
} // namespace rpgss
