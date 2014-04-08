#include <cmath>
#include <cstring>

#include "ByteArray.hpp"


namespace rpgss {
    namespace core {

        //-----------------------------------------------------------------
        ByteArray::Ptr
        ByteArray::New(int size)
        {
            ByteArray::Ptr o = new ByteArray();
            if (size > 0) {
                o->resize(size);
            }
            return o;
        }

        //-----------------------------------------------------------------
        ByteArray::Ptr
        ByteArray::New(int size, u8 value)
        {
            ByteArray::Ptr o = new ByteArray();
            if (size > 0) {
                o->resize(size, value);
            }
            return o;
        }

        //-----------------------------------------------------------------
        ByteArray::Ptr
        ByteArray::New(const u8* buffer, int bufferSize)
        {
            ByteArray::Ptr o = new ByteArray();
            if (bufferSize > 0) {
                o->reset(buffer, bufferSize);
            }
            return o;
        }

        //-----------------------------------------------------------------
        ByteArray::Ptr
        ByteArray::New(const ByteArray* that)
        {
            ByteArray::Ptr o = new ByteArray();
            if (that->_size > 0) {
                o->reset(that->_buffer, that->_size);
            }
            return o;
        }

        //-----------------------------------------------------------------
        ByteArray::ByteArray()
            : _buffer(0)
            , _size(0)
        {
        }

        //-----------------------------------------------------------------
        ByteArray::~ByteArray()
        {
            clear();
        }

        //-----------------------------------------------------------------
        std::string
        ByteArray::toString() const
        {
            if (_size > 0) {
                return std::string((const char*)_buffer, _size);
            } else {
                return std::string();
            }
        }

        //-----------------------------------------------------------------
        void
        ByteArray::resize(int size)
        {
            if (size <= 0) {
                clear();
                return;
            }
            if (size != _size) {
                u8* new_buffer = new u8[size];
                if (_size > 0) {
                    int can_copy = std::min(_size, size);
                    std::memcpy(new_buffer, _buffer, can_copy);
                }
                delete[] _buffer;
                _buffer = new_buffer;
                _size = size;
            }
        }

        //-----------------------------------------------------------------
        void
        ByteArray::resize(int size, u8 fillValue)
        {
            int old_size = _size;
            resize(size);
            int new_size = _size;
            if (old_size < new_size) {
                std::memset(_buffer + old_size, fillValue, new_size - old_size);
            }
        }

        //-----------------------------------------------------------------
        void
        ByteArray::reset(const u8* buffer, int bufferSize)
        {
            if (bufferSize <= 0) {
                clear();
                return;
            }
            if (_size == bufferSize) {
                std::memcpy(_buffer, buffer, bufferSize);
                return;
            }
            clear();
            _buffer = new u8[bufferSize];
            _size = bufferSize;
            std::memcpy(_buffer, buffer, bufferSize);
        }

        //-----------------------------------------------------------------
        ByteArray::Ptr
        ByteArray::concat(const ByteArray* that) const
        {
            ByteArray::Ptr temp = New(_size + that->_size);
            if (_size > 0) {
                std::memcpy(temp->_buffer, _buffer, _size);
            }
            if (that->_size > 0) {
                std::memcpy(temp->_buffer + _size, that->_buffer, that->_size);
            }
            return temp;
        }

        //-----------------------------------------------------------------
        void
        ByteArray::append(const ByteArray* that)
        {
            if (that->_size > 0) {
                ByteArray::Ptr temp = New(_size + that->_size);
                if (_size > 0) {
                    std::memcpy(temp->_buffer, _buffer, _size);
                }
                std::memcpy(temp->_buffer + _size, that->_buffer, that->_size);
                swap(temp);
            }
        }

        //-----------------------------------------------------------------
        void
        ByteArray::clear()
        {
            if (_buffer) {
                delete[] _buffer;
                _buffer = 0;
                _size   = 0;
            }
        }

        //-----------------------------------------------------------------
        void
        ByteArray::memset(u8 value)
        {
            if (_size > 0) {
                std::memset(_buffer, value, _size);
            }
        }

        //-----------------------------------------------------------------
        void
        ByteArray::swap(ByteArray* that)
        {
            std::swap(_buffer, that->_buffer);
            std::swap(_size, that->_size);
        }

    } // namespace core
} // namespace rpgss
