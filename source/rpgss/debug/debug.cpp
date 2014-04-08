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
