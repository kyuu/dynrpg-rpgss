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

#ifndef RPGSS_IO_READER_HPP_INCLUDED
#define RPGSS_IO_READER_HPP_INCLUDED

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "../core/ByteArray.hpp"
#include "File.hpp"


namespace rpgss {
    namespace io {

        class Reader : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<Reader> Ptr;

        public:
            static Reader::Ptr New(File* inputStream);

        public:
            File* getInputStream();

            void skipBytes(int count);
            u8 readByte();
            core::ByteArray::Ptr readByteArray(int size);

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
            explicit Reader(File* inputStream); // use New()
            ~Reader();

        private:
            File::Ptr _inputStream;
        };

        //--------------------------------------------------------------
        inline File*
        Reader::getInputStream() {
            return _inputStream;
        }

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_READER_HPP_INCLUDED
