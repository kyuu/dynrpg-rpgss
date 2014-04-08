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
