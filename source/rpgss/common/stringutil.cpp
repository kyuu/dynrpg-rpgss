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

#include <sstream>
#include <algorithm>
#include "stringutil.hpp"


namespace rpgss {

    //---------------------------------------------------------
    void SplitString(const std::string& s, char delim, std::vector<std::string>& result)
    {
        result.clear();
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            result.push_back(item);
        }
    }

    //---------------------------------------------------------
    std::vector<std::string> SplitString(const std::string& s, char delim)
    {
        std::vector<std::string> result;
        SplitString(s, delim, result);
        return result;
    }

    //---------------------------------------------------------
    void SplitString(const std::string& s, const std::string& delim, std::vector<std::string>& result, bool keep_empty)
    {
        result.clear();

        if (delim.empty()) {
            result.push_back(s);
            return;
        }

        std::string::const_iterator substart = s.begin();
        std::string::const_iterator subend;

        while (true) {
            subend = std::search(substart, s.end(), delim.begin(), delim.end());
            std::string temp(substart, subend);
            if (keep_empty || !temp.empty()) {
                result.push_back(temp);
            }
            if (subend == s.end()) {
                break;
            }
            substart = subend + delim.size();
        }
    }

    //---------------------------------------------------------
    std::vector<std::string> SplitString(const std::string& s, const std::string& delim, bool keep_empty)
    {
        std::vector<std::string> result;
        SplitString(s, delim, result, keep_empty);
        return result;
    }

    //---------------------------------------------------------
    std::string ReplaceString(std::string s, const std::string& search, const std::string& replace)
    {
        size_t pos = 0;
        while ((pos = s.find(search, pos)) != std::string::npos) {
             s.replace(pos, search.length(), replace);
             pos += replace.length();
        }
        return s;
    }

} // namespace rpgss
