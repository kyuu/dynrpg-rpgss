#include <cassert>
#include <algorithm>
#include "Reader.hpp"


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
        Reader::Ptr
        Reader::New(File* inputStream)
        {
            assert(inputStream);
            return new Reader(inputStream);
        }

        //--------------------------------------------------------------
        Reader::Reader(File* inputStream)
            : _inputStream(inputStream)
        {
        }

        //--------------------------------------------------------------
        Reader::~Reader()
        {
        }

        //--------------------------------------------------------------
        void
        Reader::skipBytes(int count)
        {
            if (count > 0) {
                _inputStream->seek(count, File::Current);
            }
        }

        //--------------------------------------------------------------
        u8
        Reader::readByte()
        {
            return readUint8();
        }

        //--------------------------------------------------------------
        core::ByteArray::Ptr
        Reader::readByteArray(int size)
        {
            core::ByteArray::Ptr byteArray = core::ByteArray::New();
            if (size > 0) {
                byteArray->resize(size);
                _inputStream->read(byteArray->getBuffer(), size);
            }
            return byteArray;
        }

        //--------------------------------------------------------------
        bool
        Reader::readBool()
        {
            return (bool)readUint8();
        }

        //--------------------------------------------------------------
        i8
        Reader::readInt8()
        {
            i8 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i16
        Reader::readInt16()
        {
            i16 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i16
        Reader::readInt16BE()
        {
            i16 n = readInt16();
            _s2((_2*)&n);
            return n;
        }

        //--------------------------------------------------------------
        i32
        Reader::readInt32()
        {
            i32 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i32
        Reader::readInt32BE()
        {
            i32 n = readInt32();
            _s4((_4*)&n);
            return n;
        }

        //--------------------------------------------------------------
        i64
        Reader::readInt64()
        {
            i64 n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        i64
        Reader::readInt64BE()
        {
            i64 n = readInt64();
            _s8((_8*)&n);
            return n;
        }

        //--------------------------------------------------------------
        u8
        Reader::readUint8()
        {
            return (u8)readInt8();
        }

        //--------------------------------------------------------------
        u16
        Reader::readUint16()
        {
            return (u16)readInt16();
        }

        //--------------------------------------------------------------
        u16
        Reader::readUint16BE()
        {
            return (u16)readInt16BE();
        }

        //--------------------------------------------------------------
        u32
        Reader::readUint32()
        {
            return (u32)readInt32();
        }

        //--------------------------------------------------------------
        u32
        Reader::readUint32BE()
        {
            return (u32)readInt32BE();
        }

        //--------------------------------------------------------------
        u64
        Reader::readUint64()
        {
            return (u64)readInt64();
        }

        //--------------------------------------------------------------
        u64
        Reader::readUint64BE()
        {
            return (u64)readInt64BE();
        }

        //--------------------------------------------------------------
        float
        Reader::readFloat()
        {
            float n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        float
        Reader::readFloatBE()
        {
            float n = readFloat();
            _s4((_4*)&n);
            return n;
        }

        //--------------------------------------------------------------
        double
        Reader::readDouble()
        {
            double n = 0;
            _inputStream->read((u8*)&n, sizeof(n));
            return n;
        }

        //--------------------------------------------------------------
        double
        Reader::readDoubleBE()
        {
            double n = readDouble();
            _s8((_8*)&n);
            return n;
        }

        //--------------------------------------------------------------
        std::string
        Reader::readString(int size)
        {
            if (size <= 0) {
                return std::string();
            }
            core::ByteArray::Ptr stringBuffer = core::ByteArray::New(size + 1);
            stringBuffer->at(size) = '\0';
            _inputStream->read(stringBuffer->getBuffer(), size);
            return std::string((const char*)stringBuffer->getBuffer());
        }

    } // namespace io
} // namespace rpgss
