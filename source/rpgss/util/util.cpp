#include "util.hpp"


namespace rpgss {
    namespace util {

        //--------------------------------------------------------------
        bool Compress(const u8* buffer, int size, io::File* ostream)
        {
            Compressor compr;
            return compr.compress(buffer, size, ostream) && compr.finish(ostream);
        }

        //--------------------------------------------------------------
        bool Decompress(const u8* buffer, int size, io::File* ostream)
        {
            Decompressor decompr;
            return decompr.decompress(buffer, size, ostream) && decompr.finish(ostream);
        }

    } // namespace util
} // namespace rpgss
