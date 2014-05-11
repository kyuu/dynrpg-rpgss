/*
    The MIT License (MIT)

    Copyright (c) 2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#ifndef RPGSS_IO_WRITER_HPP_INCLUDED
#define RPGSS_IO_WRITER_HPP_INCLUDED

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "../core/ByteArray.hpp"
#include "File.hpp"


namespace rpgss {
    namespace io {

        class Writer : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<Writer> Ptr;

        public:
            static Writer::Ptr New(File* outputStream);

        public:
            File* getOutputStream();

            void writeByte(u8 byte, int repeat = 1);
            void writeByteArray(core::ByteArray* byteArray);

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
            explicit Writer(File* outputStream); // use New()
            ~Writer();

        private:
            File::Ptr _outputStream;
        };

        //--------------------------------------------------------------
        inline File*
        Writer::getOutputStream() {
            return _outputStream;
        }

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_WRITER_HPP_INCLUDED
