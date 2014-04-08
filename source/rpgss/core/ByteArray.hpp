#ifndef RPGSS_CORE_BYTEARRAY_HPP_INCLUDED
#define RPGSS_CORE_BYTEARRAY_HPP_INCLUDED

#include <string>

#include "../common/types.hpp"
#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"


namespace rpgss {
    namespace core {

        class ByteArray : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<ByteArray> Ptr;

        public:
            static ByteArray::Ptr New(int size = 0);
            static ByteArray::Ptr New(int size, u8 value);
            static ByteArray::Ptr New(const u8* buffer, int bufferSize);
            static ByteArray::Ptr New(const ByteArray* that);

        public:
            int getSize() const;
            u8* getBuffer();
            const u8* getBuffer() const;
            u8& at(int n);
            u8 at(int n) const;

            std::string toString() const;

            void resize(int size);
            void resize(int size, u8 fillValue);
            void reset(const u8* buffer, int bufferSize);
            ByteArray::Ptr concat(const ByteArray* that) const;
            void append(const ByteArray* that);
            void memset(u8 value);
            void clear();
            void swap(ByteArray* that);

        private:
            ByteArray(); // use New()
            ~ByteArray();

        private:
            u8* _buffer;
            int _size;
        };

        //-----------------------------------------------------------------
        inline int
        ByteArray::getSize() const
        {
            return _size;
        }

        //-----------------------------------------------------------------
        inline u8*
        ByteArray::getBuffer()
        {
            return _buffer;
        }

        //-----------------------------------------------------------------
        inline const u8*
        ByteArray::getBuffer() const
        {
            return _buffer;
        }

        //-----------------------------------------------------------------
        inline u8&
        ByteArray::at(int n)
        {
            return _buffer[n];
        }

        //-----------------------------------------------------------------
        inline u8
        ByteArray::at(int n) const
        {
            return _buffer[n];
        }

    } // namespace core
} // namespace rpgss


#endif
