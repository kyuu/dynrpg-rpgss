#ifndef RPGSS_DEBUG_GUARD_HPP_INCLUDED
#define RPGSS_DEBUG_GUARD_HPP_INCLUDED

#include <string>


namespace rpgss {
    namespace debug {

        class Guard {
        public:
            explicit Guard(const std::string& blockName);
            ~Guard();

        private:
            // forbid
            Guard(const Guard&);
            void* operator new(size_t);

        private:
            std::string _blockName;
        };

    } // namespace debug
} // namespace rpgss


#endif // RPGSS_DEBUG_GUARD_HPP_INCLUDED
