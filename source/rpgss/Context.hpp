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

#ifndef RPGSS_CONTEXT_HPP_INCLUDED
#define RPGSS_CONTEXT_HPP_INCLUDED

#include "common/RefCountedObject.hpp"
#include "common/RefCountedObjectPtr.hpp"
#include "script/lua_include.hpp"


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
