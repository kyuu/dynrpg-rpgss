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

#ifndef RPGSS_REFCOUNTEDOBJECTPTR_HPP_INCLUDED
#define RPGSS_REFCOUNTEDOBJECTPTR_HPP_INCLUDED

#include <cstddef>


namespace rpgss {

    template<class T>
    class RefCountedObjectPtr {
        template<class S>
        friend class RefCountedObjectPtr;

    public:
        RefCountedObjectPtr() : _t(0) {
        }

        RefCountedObjectPtr(T* t) : _t(t) {
            if (_t) {
                _t->grabRef();
            }
        }

        RefCountedObjectPtr(const RefCountedObjectPtr& that) : _t(that._t) {
            if (_t) {
                _t->grabRef();
            }
        }

        template <class S>
        RefCountedObjectPtr(S* s) : _t(0) {
            _t = static_cast<T*>(s);
            if (_t) {
                _t->grabRef();
            }
        }

        template <class S>
        RefCountedObjectPtr(const RefCountedObjectPtr<S>& that) : _t(0) {
            _t = static_cast<T*>(that._t);
            if (_t) {
                _t->grabRef();
            }
        }

        ~RefCountedObjectPtr() {
            if (_t) {
                _t->dropRef();
            }
        }

        RefCountedObjectPtr& operator=(T* t) {
            if (_t != t) {
                if (_t) {
                    _t->dropRef();
                }
                _t = t;
                if (_t) {
                    _t->grabRef();
                }
            }
            return *this;
        }

        RefCountedObjectPtr& operator=(const RefCountedObjectPtr& that) {
            return (*this = that._t);
        }

        template <class S>
        RefCountedObjectPtr& operator=(S* s) {
            return (*this = static_cast<T*>(s));
        }

        template <class S>
        RefCountedObjectPtr& operator=(const RefCountedObjectPtr<S>& that) {
            return (*this = static_cast<T*>(that._t));
        }

        bool operator==(const RefCountedObjectPtr& that) {
            return _t == that._t;
        }

        bool operator!=(const RefCountedObjectPtr& that) {
            return _t != that._t;
        }

        T* operator->() const {
            return _t;
        }

        bool operator!() const {
            return _t == 0;
        }

        operator bool() const {
            return _t != 0;
        }

        operator T*() {
            return _t;
        }

        operator const T*() const {
            return _t;
        }

        T* get() const {
            return _t;
        }

        void clear() {
            if (_t) {
                _t->dropRef();
                _t = 0;
            }
        }

    private:
        // forbid heap allocation
        void* operator new(size_t);

    private:
        T* _t;
    };

} // namespace rpgss


#endif
