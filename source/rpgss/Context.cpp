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

#include "debug/debug.hpp"
#include "script/script.hpp"
#include "version.hpp"
#include "Context.hpp"


namespace rpgss {

    //---------------------------------------------------------
    RefCountedObjectPtr<Context> Context::_current;

    //---------------------------------------------------------
    bool
    Context::Open()
    {
        _current = new Context();

        if (!_current->init()) {
            _current.clear();
            return false;
        }

        return true;
    }

    //---------------------------------------------------------
    void
    Context::Close()
    {
        _current.clear();
    }

    //---------------------------------------------------------
    Context&
    Context::Current()
    {
        assert(_current.get());
        return *_current.get();
    }

    //---------------------------------------------------------
    Context::Context()
        : _L(0)
    {
    }

    //---------------------------------------------------------
    Context::~Context()
    {
        if (_L) {
            lua_close(_L);
        }
    }

    //---------------------------------------------------------
    bool
    Context::init()
    {
        return (
            initInterpreter() &&
            registerLibrary()
        );
    }

    //---------------------------------------------------------
    bool
    Context::initInterpreter()
    {
        // create lua state
        _L = luaL_newstate();

        // register panic handler
        lua_atpanic(_L, script::luax_panic_handler);

        // register standard lua libraries
        luaL_openlibs(_L);

        // overwrite standard print function
        lua_pushcfunction(_L, script::luax_print);
        lua_setglobal(_L, "print");

        // overwrite standard loadfile function
        lua_pushcfunction(_L, script::luax_loadfile);
        lua_setglobal(_L, "loadfile");

        // overwrite standard dofile function
        lua_pushcfunction(_L, script::luax_dofile);
        lua_setglobal(_L, "dofile");

        // register module loader (currently lua chunks only)
        lua_getglobal(_L, "package");
        if (lua_istable(_L, -1)) {
            lua_pushstring(_L, "loaders");
            lua_rawget(_L, -2); // get from package table
            if (lua_istable(_L, -1) == 0) {
                // try package.searchers
                lua_pop(_L, 1); // pop result of last lua_rawget()
                lua_pushstring(_L, "searchers");
                lua_rawget(_L, -2); // get from package table
            }
            if (lua_istable(_L, -1)) {
                lua_pushcfunction(_L, script::luax_loadfile);
                lua_rawseti(_L, -2, 1); // set in loaders table
            } else {
                // treat not being able to register our loader as fatal error
                lua_pop(_L, 2); // pop loaders and package tables
                return false;
            }
            lua_pop(_L, 1); // pop loaders table
        }
        lua_pop(_L, 1); // pop package table

        return true;
    }

    //---------------------------------------------------------
    bool
    Context::registerLibrary()
    {
        luabridge::setGlobal(_L, RPGSS_VERSION,     "_RPGSS_VERSION"    );
        luabridge::setGlobal(_L, RPGSS_VERSION_NUM, "_RPGSS_VERSION_NUM");

        script::RegisterModules(_L);

        return true;
    }

} // namespace rpgss
