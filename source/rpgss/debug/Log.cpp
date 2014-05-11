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

#include <ctime>

#include "../common/stringutil.hpp"
#include "Log.hpp"


namespace rpgss {
    namespace debug {

        //---------------------------------------------------------
        std::ofstream Log::s_outputStream;
        int Log::s_indentLevel = 0;

        //---------------------------------------------------------
        void
        Log::Open(const std::string& filename) {
            s_outputStream.open(filename.c_str());
        }

        //---------------------------------------------------------
        void
        Log::IncrementIndent() {
            s_indentLevel++;
        }

        //---------------------------------------------------------
        void
        Log::DecrementIndent() {
            s_indentLevel--;
        }

        //---------------------------------------------------------
        Log::Log()
            : _linePrefix("")
            , _lineIndentLevel(0)
        {
        }

        //---------------------------------------------------------
        Log::Log(int lineIndentLevel)
            : _linePrefix("")
            , _lineIndentLevel(lineIndentLevel)
        {
        }

        //---------------------------------------------------------
        Log::Log(const std::string& linePrefix, int lineIndentLevel)
            : _linePrefix(linePrefix)
            , _lineIndentLevel(lineIndentLevel)
        {
        }

        //---------------------------------------------------------
        Log::~Log() {
            std::ostringstream oss;
            oss << buildCurrentTimeString() << ": ";
            oss << std::string(s_indentLevel * 4, ' ');
            if (!_linePrefix.empty()) {
                oss << _linePrefix << " ";
            }
            if (_lineIndentLevel > 0) {
                oss << std::string(_lineIndentLevel * 4, ' ');
            }

            std::vector<std::string> lines;
            SplitString(_msg.str(), '\n', lines);

            for (size_t i = 0; i < lines.size(); i++) {
                s_outputStream << oss.str() << lines[i] << std::endl;
            }

            s_outputStream.flush();
        }

        //---------------------------------------------------------
        std::string
        Log::buildCurrentTimeString() const
        {
            char buffer[30];
            std::time_t now = std::time(0);
            std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", std::localtime(&now));
            return std::string((const char*)buffer);
        }

    } // namespace debug
} // namespace rpgss
