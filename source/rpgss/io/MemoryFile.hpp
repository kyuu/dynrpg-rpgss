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

#ifndef RPGSS_IO_MEMORYFILE_HPP_INCLUDED
#define RPGSS_IO_MEMORYFILE_HPP_INCLUDED

#include "../core/ByteArray.hpp"
#include "File.hpp"


namespace rpgss {
    namespace io {

        class MemoryFile : public File {
        public:
            typedef RefCountedObjectPtr<MemoryFile> Ptr;

        public:
            static MemoryFile::Ptr New(int capacity = 0);
            static MemoryFile::Ptr New(int capacity, u8 value);
            static MemoryFile::Ptr New(const u8* buffer, int bufferSize);
            static MemoryFile::Ptr New(const core::ByteArray* byteArray);

        public:
            int getCapacity() const;
            u8* getBuffer();
            const u8* getBuffer() const;

            void reserve(int capacity);
            void reserve(int capacity, u8 fillValue);
            void clear();

            bool isOpen() const;
            int  getSize() const;
            void close();
            bool eof() const;
            bool error() const;
            bool good() const;
            void clearerr();
            bool seek(int offset, SeekMode mode = Begin);
            int  tell();
            int  read(u8* buffer, int bufferSize);
            int  write(const u8* buffer, int bufferSize);
            bool flush();

        private:
            MemoryFile(); // use New()
            ~MemoryFile();

        private:
            core::ByteArray::Ptr _buffer;
            int _size;
            int _spos;
            bool _eof;
            bool _error;
        };

        //-----------------------------------------------------------------
        inline int
        MemoryFile::getCapacity() const {
            return _buffer->getSize();
        }

        //-----------------------------------------------------------------
        inline u8*
        MemoryFile::getBuffer() {
            return _buffer->getBuffer();
        }

        //-----------------------------------------------------------------
        inline const u8*
        MemoryFile::getBuffer() const {
            return _buffer->getBuffer();
        }

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_MEMORYFILE_HPP_INCLUDED
