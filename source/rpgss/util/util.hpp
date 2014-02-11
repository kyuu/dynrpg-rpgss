#ifndef RPGSS_UTIL_HPP_INCLUDED
#define RPGSS_UTIL_HPP_INCLUDED

#include <vector>
#include <string>


namespace rpgss {
    namespace util {

        void SplitString(const std::string& s, char delim, std::vector<std::string>& result);
        std::vector<std::string> SplitString(const std::string& s, char delim);

        void SplitString(const std::string& s, const std::string& delim, std::vector<std::string>& result, bool keep_empty = true);
        std::vector<std::string> SplitString(const std::string& s, const std::string& delim, bool keep_empty = true);

        std::string ReplaceString(std::string s, const std::string& search, const std::string& replace);

    } // namespace util
} // namespace rpgss

#endif // RPGSS_UTIL_HPP_INCLUDED
