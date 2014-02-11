#include <cassert>
#include <cmath>
#include <cstring>
#include "MemoryStream.hpp"


namespace rpgss {
    namespace io {

        //-----------------------------------------------------------------
        MemoryStream::Ptr
        MemoryStream::New(int capacity)
        {
            MemoryStream::Ptr o = new MemoryStream();
            if (capacity > 0) {
                o->_buffer->resize(capacity);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryStream::Ptr
        MemoryStream::New(int capacity, u8 value)
        {
            MemoryStream::Ptr o = new MemoryStream();
            if (capacity > 0) {
                o->_buffer->resize(capacity);
                o->_buffer->memset(value);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryStream::Ptr
        MemoryStream::New(const u8* buffer, int bufferSize)
        {
            assert(buffer);
            MemoryStream::Ptr o = new MemoryStream();
            if (bufferSize > 0) {
                o->write(buffer, bufferSize);
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryStream::Ptr
        MemoryStream::New(const ByteArray* byteArray)
        {
            assert(byteArray);
            MemoryStream::Ptr o = new MemoryStream();
            if (byteArray->getSize() > 0) {
                o->write(byteArray->getBuffer(), byteArray->getSize());
            }
            return o;
        }

        //-----------------------------------------------------------------
        MemoryStream::MemoryStream()
            : _size(0)
            , _spos(0)
            , _eof(false)
            , _error(false)
        {
            _buffer = ByteArray::New();
        }

        //-----------------------------------------------------------------
        MemoryStream::~MemoryStream()
        {
        }

        //-----------------------------------------------------------------
        void
        MemoryStream::reserve(int capacity)
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
        MemoryStream::reserve(int capacity, u8 fillValue)
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
        MemoryStream::clear()
        {
            _buffer->clear();
            _size  = 0;
            _spos  = 0;
            _eof   = false;
            _error = false;
        }

        //-----------------------------------------------------------------
        bool
        MemoryStream::eof() const
        {
            return _eof;
        }

        //-----------------------------------------------------------------
        bool
        MemoryStream::error() const
        {
            return _error;
        }

        //-----------------------------------------------------------------
        bool
        MemoryStream::good() const
        {
            return !eof() && !error();
        }

        //-----------------------------------------------------------------
        void
        MemoryStream::clearerr()
        {
            _eof = false;
            _error = false;
        }

        //-----------------------------------------------------------------
        bool
        MemoryStream::seek(int offset, SeekMode mode)
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
        MemoryStream::tell()
        {
            return (int)_spos;
        }

        //-----------------------------------------------------------------
        int
        MemoryStream::read(u8* buffer, int bufferSize)
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
        MemoryStream::write(const u8* buffer, int bufferSize)
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
        MemoryStream::flush()
        {
            return true;
        }

    } // namespace io
} // namespace rpgss
