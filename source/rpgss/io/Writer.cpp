#include <cassert>
#include <algorithm>
#include "Writer.hpp"


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
        Writer::Ptr
        Writer::New(File* outputStream)
        {
            assert(outputStream);
            return new Writer(outputStream);
        }

        //--------------------------------------------------------------
        Writer::Writer(File* outputStream)
            : _outputStream(outputStream)
        {
        }

        //--------------------------------------------------------------
        Writer::~Writer()
        {
        }

        //--------------------------------------------------------------
        void
        Writer::writeByte(u8 byte, int repeat)
        {
            for (int i = 0; i < repeat; i++)
            {
                writeUint8(byte);
            }
        }

        //--------------------------------------------------------------
        void
        Writer::writeByteArray(core::ByteArray* byteArray)
        {
            if (byteArray->getSize() > 0) {
                _outputStream->write(byteArray->getBuffer(), byteArray->getSize());
            }
        }

        //--------------------------------------------------------------
        void
        Writer::writeBool(bool b)
        {
            writeUint8((u8)b);
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt8(i8 n)
        {
            _outputStream->write((u8*)&n, sizeof(n));
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt16(i16 n)
        {
            _outputStream->write((u8*)&n, sizeof(n));
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt16BE(i16 n)
        {
            _s2((_2*)&n);
            writeInt16(n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt32(i32 n)
        {
            _outputStream->write((u8*)&n, sizeof(n));
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt32BE(i32 n)
        {
            _s4((_4*)&n);
            writeInt32(n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt64(i64 n)
        {
            _outputStream->write((u8*)&n, sizeof(n));
        }

        //--------------------------------------------------------------
        void
        Writer::writeInt64BE(i64 n)
        {
            _s8((_8*)&n);
            writeInt64(n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint8(u8 n)
        {
            writeInt8((i8)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint16(u16 n)
        {
            writeInt16((i16)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint16BE(u16 n)
        {
            writeInt16BE((i16)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint32(u32 n)
        {
            writeInt32((i32)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint32BE(u32 n)
        {
            writeInt32BE((i32)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint64(u64 n)
        {
            writeInt64((i64)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeUint64BE(u64 n)
        {
            writeInt64BE((i64)n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeFloat(float n)
        {
            _outputStream->write((u8*)&n, sizeof(n));
        }

        //--------------------------------------------------------------
        void
        Writer::writeFloatBE(float n)
        {
            _s4((_4*)&n);
            writeFloat(n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeDouble(double n)
        {
            _outputStream->write((u8*)&n, sizeof(n));
        }

        //--------------------------------------------------------------
        void
        Writer::writeDoubleBE(double n)
        {
            _s8((_8*)&n);
            writeDouble(n);
        }

        //--------------------------------------------------------------
        void
        Writer::writeString(const std::string& s)
        {
            if (s.size() > 0) {
                _outputStream->write((u8*)s.c_str(), s.size());
            }
        }

    } // namespace io
} // namespace rpgss
