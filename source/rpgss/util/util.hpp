#ifndef RPGSS_UTIL_UTIL_HPP_INCLUDED
#define RPGSS_UTIL_UTIL_HPP_INCLUDED

#include "../io/io.hpp"
#include "Compressor.hpp"
#include "Decompressor.hpp"


namespace rpgss {
    namespace util {

        bool Compress(const u8* buffer, int size, io::File* ostream);
        bool Decompress(const u8* buffer, int size, io::File* ostream);

    } // namespace util
} // namespace rpgss


#endif // RPGSS_UTIL_UTIL_HPP_INCLUDED
