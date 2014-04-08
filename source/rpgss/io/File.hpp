#ifndef RPGSS_IO_FILE_HPP_INCLUDED
#define RPGSS_IO_FILE_HPP_INCLUDED

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"
#include "File.hpp"


namespace rpgss {
    namespace io {

        class File : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<File> Ptr;

        public:
            enum OpenMode {
                In,
                Out,
                Append,
            };

            enum SeekMode {
                Begin,
                Current,
                End,
            };

            virtual bool isOpen() const = 0;
            virtual int getSize() const = 0;
            virtual void close() = 0;
            virtual bool eof() const = 0;
            virtual bool error() const = 0;
            virtual bool good() const = 0;
            virtual void clearerr() = 0;
            virtual bool seek(int offset, SeekMode mode = Begin) = 0;
            virtual int  tell() = 0;
            virtual int  read(u8* buffer, int size) = 0;
            virtual int  write(const u8* buffer, int size) = 0;
            virtual bool flush() = 0;

        protected:
            virtual ~File() { }
        };

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_FILE_HPP_INCLUDED
