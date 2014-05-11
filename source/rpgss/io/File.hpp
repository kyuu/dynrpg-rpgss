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
