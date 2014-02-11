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
