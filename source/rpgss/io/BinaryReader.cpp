#include <cassert>
#include <algorithm>
#include "BinaryReader.hpp"


namespace rpgss {
    namespace io {

        union _2 {
            u16 value;
            u8  bytes[2];
        };

        union _4 {
            u32 value;
            u8  bytes[4];
        };

        union _8 {
            u64 value;
            u8  bytes[8];
        };

        inline void _s2(_2* p)
        {
            _2 w;
            w.value = p->value;
            p->bytes[0] = w.bytes[1];
            p->bytes[1] = w.bytes[0];
        }

        inline void _s4(_4* p)
        {
            _4 d;
            d.value = p->value;
            p->bytes[0] = d.bytes[3];
            p->bytes[1] = d.bytes[2];
            p->bytes[2] = d.bytes[1];
            p->bytes[3] = d.bytes[0];
        }

        inline void _s8(_8* p)
        {
            _8 q;
            q.value = p->value;
            p->bytes[0] = q.bytes[7];
            p->bytes[1] = q.bytes[6];
            p->bytes[2] = q.bytes[5];
            p->bytes[3] = q.bytes[4];
            p->bytes[4] = q.bytes[3];
            p->bytes[5] = q.bytes[2];
            p->bytes[6] = q.bytes[1];
            p->bytes[7] = q.bytes[0];
        }

        //--------------------------------------------------------------
        BinaryReader::Ptr
        BinaryReader::New(Stream* inputStream)
        {
            assert(inputStream);
            return new BinaryReader(inputStream);
        }

        //--------------------------------------------------------------
        BinaryReader::BinaryReader(Stream* inputStream)
            : _inputStream(inputStream)
        {
        }

        //--------------------------------------------------------------
        BinaryReader::~BinaryReader()
        {
        }

        //--------------------------------------------------------------
        void
        BinaryReader::skipBytes(int count)
        {
            if (count > 0) {
                _inputStream->seek(count, Stream::Current);
            }
        }

        //--------------------------------------------------------------
        u8
        BinaryReader::readByte()
        {
            return readUint8();
        }

        //--------------------------------------------------------------
        ByteArray::Ptr
        BinaryReader::readByteArray(int size)
        {
            ByteArray::Ptr byteArray = ByteArray::New();
            if (size > 0) {
                byteArray->resize(size);
                _inputStream->read(byteArray->getBuffer(), size);
            }
            return byteArray;
        }

        //--------------------------------------------------------------
        bool
        BinaryReader::readBool()
        {
            return (bool)readUint8();
        }

        //--------------------------------------------------------------
        i8
        BinaryReader::readInt8()
        {
            i8 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i16
        BinaryReader::readInt16()
        {
            i16 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i16
        BinaryReader::readInt16BE()
        {
            i16 n = readInt16();
            _s2((_2*)&n);
            return n;
        }

        //--------------------------------------------------------------
        i32
        BinaryReader::readInt32()
        {
            i32 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i32
        BinaryReader::readInt32BE()
        {
            i32 n = readInt32();
            _s4((_4*)&n);
            return n;
        }

        //--------------------------------------------------------------
        i64
        BinaryReader::readInt64()
        {
            i64 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i64
        BinaryReader::readInt64BE()
        {
            i64 n = readInt64();
            _s8((_8*)&n);
            return n;
        }

        //--------------------------------------------------------------
        u8
        BinaryReader::readUint8()
        {
            return (u8)readInt8();
        }

        //--------------------------------------------------------------
        u16
        BinaryReader::readUint16()
        {
            return (u16)readInt16();
        }

        //--------------------------------------------------------------
        u16
        BinaryReader::readUint16BE()
        {
            return (u16)readInt16BE();
        }

        //--------------------------------------------------------------
        u32
        BinaryReader::readUint32()
        {
            return (u32)readInt32();
        }

        //--------------------------------------------------------------
        u32
        BinaryReader::readUint32BE()
        {
            return (u32)readInt32BE();
        }

        //--------------------------------------------------------------
        u64
        BinaryReader::readUint64()
        {
            return (u64)readInt64();
        }

        //--------------------------------------------------------------
        u64
        BinaryReader::readUint64BE()
        {
            return (u64)readInt64BE();
        }

        //--------------------------------------------------------------
        float
        BinaryReader::readFloat()
        {
            float n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        float
        BinaryReader::readFloatBE()
        {
            float n = readFloat();
            _s4((_4*)&n);
            return n;
        }

        //--------------------------------------------------------------
        double
        BinaryReader::readDouble()
        {
            double n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        double
        BinaryReader::readDoubleBE()
        {
            double n = readDouble();
            _s8((_8*)&n);
            return n;
        }

        //--------------------------------------------------------------
        std::string
        BinaryReader::readString(int size)
        {
            if (size <= 0) {
                return std::string();
            }
            ByteArray::Ptr stringBuffer = ByteArray::New(size + 1);
            stringBuffer->at(size) = '\0';
            _inputStream->read(stringBuffer->getBuffer(), size);
            return std::string((const char*)stringBuffer->getBuffer());
        }

    } // namespace io
} // namespace rpgss
