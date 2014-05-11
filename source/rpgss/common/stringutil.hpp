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

#ifndef RPGSS_STRINGUTIL_HPP_INCLUDED
#define RPGSS_STRINGUTIL_HPP_INCLUDED

#include <vector>
#include <string>


namespace rpgss {

    void SplitString(const std::string& s, char delim, std::vector<std::string>& result);
    std::vector<std::string> SplitString(const std::string& s, char delim);

    void SplitString(const std::string& s, const std::string& delim, std::vector<std::string>& result, bool keep_empty = true);
    std::vector<std::string> SplitString(const std::string& s, const std::string& delim, bool keep_empty = true);

    std::string ReplaceString(std::string s, const std::string& search, const std::string& replace);

} // namespace rpgss

#endif // RPGSS_STRINGUTIL_HPP_INCLUDED
