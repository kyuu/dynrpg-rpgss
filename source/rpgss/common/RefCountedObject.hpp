#ifndef RPGSS_REFCOUNTEDOBJECT_HPP_INCLUDED
#define RPGSS_REFCOUNTEDOBJECT_HPP_INCLUDED

#include "RefCountedObjectPtr.hpp"


namespace rpgss {

    class RefCountedObject {
    public:
        typedef RefCountedObjectPtr<RefCountedObject> Ptr;

    public:
        void grabRef() {
            ++_refCount;
        }

        bool dropRef() {
            if (--_refCount == 0) {
                delete this;
                return true;
            }
            return false;
        }

        int getRefCount() const {
            return _refCount;
        }

    protected:
        RefCountedObject()
            : _refCount(0)
        {
        }

        virtual ~RefCountedObject() {
        }

    private:
        int _refCount;
    };

} // namespace rpgss


#endif
