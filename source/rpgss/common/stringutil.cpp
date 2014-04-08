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
