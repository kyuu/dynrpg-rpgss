#include <cassert>

#include "../../Context.hpp"
#include "FontWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            //---------------------------------------------------------
            void
            FontWrapper::Push(lua_State* L, graphics::Font* font)
            {
                assert(font);
                luabridge::push(L, FontWrapper(font));
            }

            //---------------------------------------------------------
            bool
            FontWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<FontWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            graphics::Font*
            FontWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Font expected, got nothing");
                    return 0;
                }
                FontWrapper* wrapper = luabridge::Stack<FontWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Font expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            graphics::Font*
            FontWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                FontWrapper* wrapper = luabridge::Stack<FontWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            FontWrapper::FontWrapper(graphics::Font* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            FontWrapper::get_maxCharWidth() const
            {
                return This->getMaxCharWidth();
            }

            //---------------------------------------------------------
            int
            FontWrapper::get_maxCharHeight() const
            {
                return This->getMaxCharHeight();
            }

            //---------------------------------------------------------
            int
            FontWrapper::get_tabWidth() const
            {
                return This->getTabWidth();
            }

            //---------------------------------------------------------
            void
            FontWrapper::set_tabWidth(int tabWidth)
            {
                This->setTabWidth(tabWidth);
            }

            //---------------------------------------------------------
            int
            FontWrapper::getStringWidth(lua_State* L)
            {
                size_t len;
                const char* str = luaL_checklstring(L, 2, &len);
                lua_pushnumber(L, This->getStringWidth(str, len));
                return 1;
            }

            //---------------------------------------------------------
            int
            FontWrapper::wordWrapString(lua_State* L)
            {
                size_t len;
                const char* str = luaL_checklstring(L, 2, &len);
                int max_line_width = luaL_checkint(L, 3);

                std::string text(str, len);
                std::vector<std::pair<int, int> > lines;
                This->wordWrapString(str, len, max_line_width, lines);

                luabridge::LuaRef t = luabridge::newTable(L);
                for (size_t i = 0; i < lines.size(); i++) {
                    t[i+1] = text.substr(lines[i].first, lines[i].second);
                }
                luabridge::push(L, t);

                return 1;
            }

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss
