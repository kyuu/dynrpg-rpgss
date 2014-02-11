#ifndef RPGSS_IO_BINARYREADER_HPP_INCLUDED
#define RPGSS_IO_BINARYREADER_HPP_INCLUDED

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "../core/ByteArray.hpp"
#include "Stream.hpp"


namespace rpgss {
    namespace io {

        class BinaryReader : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<BinaryReader> Ptr;

        public:
            static BinaryReader::Ptr New(Stream* inputStream);

        public:
            Stream* getInputStream();

            void skipBytes(int count);
            u8 readByte();
            ByteArray::Ptr readByteArray(int size);

            bool readBool();

            i8  readInt8();
            i16 readInt16();
            i16 readInt16BE();
            i32 readInt32();
            i32 readInt32BE();
            i64 readInt64();
            i64 readInt64BE();

            u8  readUint8();
            u16 readUint16();
            u16 readUint16BE();
            u32 readUint32();
            u32 readUint32BE();
            u64 readUint64();
            u64 readUint64BE();

            float  readFloat();
            float  readFloatBE();
            double readDouble();
            double readDoubleBE();

            std::string readString(int size);

        private:
            explicit BinaryReader(Stream* inputStream); // use New()
            ~BinaryReader();

        private:
            Stream::Ptr _inputStream;
        };

        //--------------------------------------------------------------
        inline Stream*
        BinaryReader::getInputStream() {
            return _inputStream;
        }

    } // namespace io
} // namespace rpgss


#endif
