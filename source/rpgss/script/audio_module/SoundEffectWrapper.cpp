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

#include <cassert>

#include "../../Context.hpp"
#include "SoundEffectWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace audio_module {

            //---------------------------------------------------------
            void
            SoundEffectWrapper::Push(lua_State* L, audio::SoundEffect* soundeffect)
            {
                assert(soundeffect);
                luabridge::push(L, SoundEffectWrapper(soundeffect));
            }

            //---------------------------------------------------------
            bool
            SoundEffectWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<SoundEffectWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            audio::SoundEffect*
            SoundEffectWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "SoundEffect expected, got nothing");
                    return 0;
                }
                SoundEffectWrapper* wrapper = luabridge::Stack<SoundEffectWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "SoundEffect expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            audio::SoundEffect*
            SoundEffectWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                SoundEffectWrapper* wrapper = luabridge::Stack<SoundEffectWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            SoundEffectWrapper::SoundEffectWrapper(audio::SoundEffect* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            float
            SoundEffectWrapper::get_volume() const
            {
                return This->getVolume();
            }

            //---------------------------------------------------------
            void
            SoundEffectWrapper::set_volume(float volume)
            {
                if (volume < 0.0 || volume > 1.0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->setVolume(volume);
            }

            //---------------------------------------------------------
            int
            SoundEffectWrapper::play(lua_State* L)
            {
                This->play();
                return 0;
            }

            //---------------------------------------------------------
            int
            SoundEffectWrapper::stop(lua_State* L)
            {
                This->stop();
                return 0;
            }

        } // audio_module
    } // namespace script
} // namespace rpgss
