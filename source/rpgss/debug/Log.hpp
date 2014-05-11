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

#ifndef RPGSS_DEBUG_LOG_HPP_INCLUDED
#define RPGSS_DEBUG_LOG_HPP_INCLUDED

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


namespace rpgss {
    namespace debug {

        class Log {
        public:
            static void Open(const std::string& filename);
            static void IncrementIndent();
            static void DecrementIndent();

        public:
            Log();
            explicit Log(int lineIndentLevel);
            explicit Log(const std::string& linePrefix, int lineIndentLevel = 0);
            ~Log();

            template<class T>
            Log& operator<<(T t) {
                _msg << t;
                return *this;
            }

        private:
            // forbid
            Log(const Log&);
            void* operator new(size_t);

            std::string buildCurrentTimeString() const;

        private:
            std::ostringstream _msg;
            std::string _linePrefix;
            int _lineIndentLevel;

        private:
            static std::ofstream s_outputStream;
            static int s_indentLevel;
        };

    } // namespace debug
} // namespace rpgss


#endif // RPGSS_DEBUG_LOG_HPP_INCLUDED
