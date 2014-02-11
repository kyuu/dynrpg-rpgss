#ifndef RPGSS_LUA_INCLUDE_HPP_INCLUDED
#define RPGSS_LUA_INCLUDE_HPP_INCLUDED

#ifdef RPGSS_CPP_LUA
#  include <lua.h>
#  include <lualib.h>
#  include <lauxlib.h>
#else
#  include <lua.hpp>
#endif // RPGSS_CPP_LUA

#include "../../LuaBridge/LuaBridge.h"

#endif // RPGSS_LUA_INCLUDE_HPP_INCLUDED
