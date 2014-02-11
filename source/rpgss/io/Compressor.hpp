#ifndef RPGSS_IO_COMPRESSOR_HPP_INCLUDED
#define RPGSS_IO_COMPRESSOR_HPP_INCLUDED

#include <zlib.h>

#include "../common/types.hpp"
#include "../core/ByteArray.hpp"
#include "Stream.hpp"


namespace rpgss {
    namespace io {

        class Compressor {
        public:
            Compressor();
            ~Compressor();

            bool good() const;
            bool compress(const u8* buffer, int size, Stream* ostream);
            bool finish(Stream* ostream);

        private:
            bool _good;
            z_stream _stream;
            ByteArray::Ptr _buffer;
        };

        //-----------------------------------------------------------------
        inline bool
        Compressor::good() const
        {
            return _good;
        }

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_COMPRESSOR_HPP_INCLUDED
