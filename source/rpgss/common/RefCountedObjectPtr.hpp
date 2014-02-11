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
