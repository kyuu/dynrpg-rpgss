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

#include <cassert>
#include <cmath>
#include <cstring>
#include "MemoryFile.hpp"


namespace rpgss {
    namespace io {

        //-----------------------------------------------------------------
        MemoryFile::Ptr
        MemoryFile::New(int capacity)
        {
            MemoryFile::Ptr o = new MemoryFile();
            if (capacity > 0) {
                o->_buffer->resize(capacity);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryFile::Ptr
        MemoryFile::New(int capacity, u8 value)
        {
            MemoryFile::Ptr o = new MemoryFile();
            if (capacity > 0) {
                o->_buffer->resize(capacity);
                o->_buffer->memset(value);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryFile::Ptr
        MemoryFile::New(const u8* buffer, int bufferSize)
        {
            assert(buffer);
            MemoryFile::Ptr o = new MemoryFile();
            if (bufferSize > 0) {
                o->write(buffer, bufferSize);
                o->seek(0);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryFile::Ptr
        MemoryFile::New(const core::ByteArray* byteArray)
        {
            assert(byteArray);
            MemoryFile::Ptr o = new MemoryFile();
            if (byteArray->getSize() > 0) {
                o->write(byteArray->getBuffer(), byteArray->getSize());
                o->seek(0);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryFile::MemoryFile()
            : _size(0)
            , _spos(0)
            , _eof(false)
            , _error(false)
        {
            _buffer = core::ByteArray::New();
        }

        //-----------------------------------------------------------------
        MemoryFile::~MemoryFile()
        {
        }

        //-----------------------------------------------------------------
        void
        MemoryFile::reserve(int capacity)
        {
            if (capacity > _buffer->getSize()) {
                // make sure capacity is always power of two
                capacity = 1 << ((int)std::ceil(std::log10((double)capacity) / std::log10(2.0)));
                // increase capacity
                _buffer->resize(capacity);
            }
        }

        //-----------------------------------------------------------------
        void
        MemoryFile::reserve(int capacity, u8 fillValue)
        {
            if (capacity > _buffer->getSize()) {
                // make sure capacity is always power of two
                capacity = 1 << ((int)std::ceil(std::log10((double)capacity) / std::log10(2.0)));
                // increase capacity
                _buffer->resize(capacity, fillValue);
            }
        }

        //-----------------------------------------------------------------
        void
        MemoryFile::clear()
        {
            _buffer->clear();
            _size  = 0;
            _spos  = 0;
            _eof   = false;
            _error = false;
        }

        //-----------------------------------------------------------------
        bool
        MemoryFile::isOpen() const
        {
            return true;
        }

        //-----------------------------------------------------------------
        int
        MemoryFile::getSize() const
        {
            return _size;
        }

        //-----------------------------------------------------------------
        void
        MemoryFile::close()
        {
            clear();
        }

        //-----------------------------------------------------------------
        bool
        MemoryFile::eof() const
        {
            return _eof;
        }

        //-----------------------------------------------------------------
        bool
        MemoryFile::error() const
        {
            return _error;
        }

        //-----------------------------------------------------------------
        bool
        MemoryFile::good() const
        {
            return !eof() && !error();
        }

        //-----------------------------------------------------------------
        void
        MemoryFile::clearerr()
        {
            _eof = false;
            _error = false;
        }

        //-----------------------------------------------------------------
        bool
        MemoryFile::seek(int offset, SeekMode mode)
        {
            _eof = false;

            switch (mode) {
            case Begin: {
                if (offset >= 0 && offset <= _size) {
                    _spos = offset;
                } else {
                    _error = true;
                    return false;
                }
                break;
            }
            case Current: {
                int new_spos = _spos + offset;
                if (new_spos >= 0 && new_spos <= _size) {
                    _spos = new_spos;
                } else {
                    _error = true;
                    return false;
                }
                break;
            }
            case End: {
                int new_spos = _size + offset;
                if (new_spos >= 0 && new_spos <= _size) {
                    _spos = new_spos;
                } else {
                    _error = true;
                    return false;
                }
                break;
            }
            default:
                return false;
            }

            return true;
        }

        //-----------------------------------------------------------------
        int
        MemoryFile::tell()
        {
            return (int)_spos;
        }

        //-----------------------------------------------------------------
        int
        MemoryFile::read(u8* buffer, int bufferSize)
        {
            assert(buffer);
            if (_eof || bufferSize == 0) {
                return 0;
            }
            if (_spos >= _size) {
                _eof = true;
                return 0;
            }
            int can_read = std::min(_size - _spos, bufferSize);
            std::memcpy(buffer, _buffer->getBuffer() + _spos, can_read);
            _spos += can_read;
            if (can_read < bufferSize) {
                _eof = true;
            }
            return can_read;
        }

        //-----------------------------------------------------------------
        int
        MemoryFile::write(const u8* buffer, int bufferSize)
        {
            assert(buffer);
            if (bufferSize == 0) {
                return 0;
            }
            if (_spos + bufferSize > _size) {
                int new_size = _spos + bufferSize;
                reserve(new_size);
                _size = new_size;
            }
            std::memcpy(_buffer->getBuffer() + _spos, buffer, bufferSize);
            _spos += bufferSize;
            return bufferSize;
        }

        //-----------------------------------------------------------------
        bool
        MemoryFile::flush()
        {
            return true;
        }

    } // namespace io
} // namespace rpgss
