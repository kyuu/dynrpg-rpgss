#ifndef RPGSS_IO_BINARYWRITER_HPP_INCLUDED
#define RPGSS_IO_BINARYWRITER_HPP_INCLUDED

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "../core/ByteArray.hpp"
#include "Stream.hpp"


namespace rpgss {
    namespace io {

        class BinaryWriter : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<BinaryWriter> Ptr;

        public:
            static BinaryWriter::Ptr New(Stream* outputStream);

        public:
            Stream* getOutputStream();

            void writeByte(u8 byte, int repeat = 1);
            void writeByteArray(ByteArray* byteArray);

            void writeBool(bool b);

            void writeInt8(i8 n);
            void writeInt16(i16 n);
            void writeInt16BE(i16 n);
            void writeInt32(i32 n);
            void writeInt32BE(i32 n);
            void writeInt64(i64 n);
            void writeInt64BE(i64 n);

            void writeUint8(u8 n);
            void writeUint16(u16 n);
            void writeUint16BE(u16 n);
            void writeUint32(u32 n);
            void writeUint32BE(u32 n);
            void writeUint64(u64 n);
            void writeUint64BE(u64 n);

            void writeFloat(float n);
            void writeFloatBE(float n);
            void writeDouble(double n);
            void writeDoubleBE(double n);

            void writeString(const std::string& s);

        private:
            explicit BinaryWriter(Stream* outputStream); // use New()
            ~BinaryWriter();

        private:
            Stream::Ptr _outputStream;
        };

        //--------------------------------------------------------------
        inline Stream*
        BinaryWriter::getOutputStream() {
            return _outputStream;
        }

    } // namespace io
} // namespace rpgss


#endif
