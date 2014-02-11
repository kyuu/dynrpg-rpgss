#ifndef RPGSS_CONTEXT_HPP_INCLUDED
#define RPGSS_CONTEXT_HPP_INCLUDED

#include "common/RefCountedObject.hpp"
#include "common/RefCountedObjectPtr.hpp"
#include "script/lua.hpp"


namespace rpgss {

    class Context : public RefCountedObject {
    public:
        static bool Open();
        static void Close();
        static Context& Current();

        lua_State* interpreter() const;

    private:
        Context();
        ~Context();

        bool init();
        bool initInterpreter();
        bool registerLibrary();

    private:
        lua_State* _L;
        static RefCountedObjectPtr<Context> _current;
    };

    //---------------------------------------------------------
    inline lua_State*
    Context::interpreter() const
    {
        return _L;
    }

} // namespace rpgss


#endif // RPGSS_CONTEXT_HPP_INCLUDED
