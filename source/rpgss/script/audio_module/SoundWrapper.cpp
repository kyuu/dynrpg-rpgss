#include <cassert>

#include "../../Context.hpp"
#include "SoundWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace audio_module {

            //---------------------------------------------------------
            void
            SoundWrapper::Push(lua_State* L, audio::Sound* sound)
            {
                assert(sound);
                luabridge::push(L, SoundWrapper(sound));
            }

            //---------------------------------------------------------
            bool
            SoundWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<SoundWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            audio::Sound*
            SoundWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Sound expected, got nothing");
                    return 0;
                }
                SoundWrapper* wrapper = luabridge::Stack<SoundWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Sound expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            audio::Sound*
            SoundWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                SoundWrapper* wrapper = luabridge::Stack<SoundWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            SoundWrapper::SoundWrapper(audio::Sound* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            bool
            SoundWrapper::get_isPlaying() const
            {
                return This->isPlaying();
            }

            //---------------------------------------------------------
            bool
            SoundWrapper::get_isSeekable() const
            {
                return This->isSeekable();
            }

            //---------------------------------------------------------
            int
            SoundWrapper::get_length() const
            {
                return This->getLength();
            }

            //---------------------------------------------------------
            bool
            SoundWrapper::get_repeat() const
            {
                return This->getRepeat();
            }

            //---------------------------------------------------------
            void
            SoundWrapper::set_repeat(bool repeat)
            {
                This->setRepeat(repeat);
            }

            //---------------------------------------------------------
            float
            SoundWrapper::get_volume() const
            {
                return This->getVolume();
            }

            //---------------------------------------------------------
            void
            SoundWrapper::set_volume(float volume)
            {
                if (volume < 0.0 || volume > 1.0) {
                    luaL_error(Context::Current().interpreter(), "invalid value");
                }
                This->setVolume(volume);
            }

            //---------------------------------------------------------
            int
            SoundWrapper::get_position() const
            {
                return This->getPosition();
            }

            //---------------------------------------------------------
            void
            SoundWrapper::set_position(int position)
            {
                if (This->isSeekable()) {
                    if (position < 1 || position > This->getLength()) {
                        luaL_error(Context::Current().interpreter(), "invalid value");
                    }
                    This->setPosition(position - 1);
                }
            }

            //---------------------------------------------------------
            int
            SoundWrapper::__len(lua_State* L)
            {
                lua_pushnumber(L, This->getLength());
                return 1;
            }

            //---------------------------------------------------------
            int
            SoundWrapper::play(lua_State* L)
            {
                This->play();
                return 0;
            }

            //---------------------------------------------------------
            int
            SoundWrapper::stop(lua_State* L)
            {
                This->stop();
                return 0;
            }

            //---------------------------------------------------------
            int
            SoundWrapper::reset(lua_State* L)
            {
                This->reset();
                return 0;
            }

        } // audio_module
    } // namespace script
} // namespace rpgss
