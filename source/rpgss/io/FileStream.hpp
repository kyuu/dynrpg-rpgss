#ifndef RPGSS_IO_FILESTREAM_HPP_INCLUDED
#define RPGSS_IO_FILESTREAM_HPP_INCLUDED

#include "Stream.hpp"


namespace rpgss {
    namespace io {

        class FileStream : public Stream {
        public:
            typedef RefCountedObjectPtr<FileStream> Ptr;

        public:
            virtual bool isOpen() const = 0;
            virtual int getSize() const = 0;
            virtual void close() = 0;

        protected:
            virtual ~FileStream() { }
        };

    } // namespace io
} // namespace rpgss


#endif
