#ifndef RPGSS_IO_STREAM_HPP_INCLUDED
#define RPGSS_IO_STREAM_HPP_INCLUDED

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"


namespace rpgss {
    namespace io {

        class Stream : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<Stream> Ptr;

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

        public:
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
            ~Stream() { }
        };

    } // namespace io
} // namespace rpgss


#endif
