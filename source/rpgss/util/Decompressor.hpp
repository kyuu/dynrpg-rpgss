#ifndef RPGSS_UTIL_DECOMPRESSOR_HPP_INCLUDED
#define RPGSS_UTIL_DECOMPRESSOR_HPP_INCLUDED

#include <zlib.h>

#include "../common/types.hpp"
#include "../core/ByteArray.hpp"
#include "../io/File.hpp"


namespace rpgss {
    namespace util {

        class Decompressor {
        public:
            Decompressor();
            ~Decompressor();

            bool good() const;
            bool decompress(const u8* buffer, int size, io::File* ostream);
            bool finish(io::File* ostream);

        private:
            bool _good;
            z_stream _stream;
            core::ByteArray::Ptr _buffer;
        };

        //-----------------------------------------------------------------
        inline bool
        Decompressor::good() const
        {
            return _good;
        }

    } // namespace util
} // namespace rpgss


#endif // RPGSS_UTIL_DECOMPRESSOR_HPP_INCLUDED
