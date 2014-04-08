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
