#ifndef RPGSS_IO_MEMORYSTREAM_HPP_INCLUDED
#define RPGSS_IO_MEMORYSTREAM_HPP_INCLUDED

#include "../core/ByteArray.hpp"
#include "Stream.hpp"


namespace rpgss {
    namespace io {

        class MemoryStream : public Stream {
        public:
            typedef RefCountedObjectPtr<MemoryStream> Ptr;

        public:
            static MemoryStream::Ptr New(int capacity = 0);
            static MemoryStream::Ptr New(int capacity, u8 value);
            static MemoryStream::Ptr New(const u8* buffer, int bufferSize);
            static MemoryStream::Ptr New(const ByteArray* byteArray);

        public:
            int getSize() const;
            int getCapacity() const;
            u8* getBuffer();
            const u8* getBuffer() const;

            void reserve(int capacity);
            void reserve(int capacity, u8 fillValue);
            void clear();

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
            MemoryStream(); // use New()
            ~MemoryStream();

        private:
            ByteArray::Ptr _buffer;
            int _size;
            int _spos;
            bool _eof;
            bool _error;
        };

        //-----------------------------------------------------------------
        inline int
        MemoryStream::getSize() const {
            return _size;
        }

        //-----------------------------------------------------------------
        inline int
        MemoryStream::getCapacity() const {
            return _buffer->getSize();
        }

        //-----------------------------------------------------------------
        inline u8*
        MemoryStream::getBuffer() {
            return _buffer->getBuffer();
        }

        //-----------------------------------------------------------------
        inline const u8*
        MemoryStream::getBuffer() const {
            return _buffer->getBuffer();
        }

    } // namespace io
} // namespace rpgss


#endif
