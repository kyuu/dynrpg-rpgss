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

#include "../io_module/io_module.hpp"
#include "audio_module.hpp"


namespace rpgss {
    namespace script {
        namespace audio_module {

            //---------------------------------------------------------
            int audio_openSound(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    // openSound(filename)
                    const char* filename = lua_tostring(L, 1);
                    audio::Sound::Ptr sound = audio::OpenSound(filename);
                    if (sound) {
                        SoundWrapper::Push(L, sound);
                    } else {
                        lua_pushnil(L);
                    }
                }
                else
                {
                    // openSound(file)
                    io::File* file = io_module::FileWrapper::Get(L, 1);
                    audio::Sound::Ptr sound = audio::OpenSound(file);
                    if (sound) {
                        SoundWrapper::Push(L, sound);
                    } else {
                        lua_pushnil(L);
                    }
                }
                return 1;
            }

            //---------------------------------------------------------
            int audio_openSoundEffect(lua_State* L)
            {
                if (lua_isstring(L, 1))
                {
                    // openSoundEffect(filename)
                    const char* filename = lua_tostring(L, 1);
                    audio::SoundEffect::Ptr sound_effect = audio::OpenSoundEffect(filename);
                    if (sound_effect) {
                        SoundEffectWrapper::Push(L, sound_effect);
                    } else {
                        lua_pushnil(L);
                    }
                }
                else
                {
                    // openSoundEffect(stream)
                    io::File* file = io_module::FileWrapper::Get(L, 1);
                    audio::SoundEffect::Ptr sound_effect = audio::OpenSoundEffect(file);
                    if (sound_effect) {
                        SoundEffectWrapper::Push(L, sound_effect);
                    } else {
                        lua_pushnil(L);
                    }
                }
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterAudioModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("audio")

                        .beginClass<SoundWrapper>("Sound")
                            .addProperty("isPlaying",           &SoundWrapper::get_isPlaying)
                            .addProperty("isSeekable",          &SoundWrapper::get_isSeekable)
                            .addProperty("length",              &SoundWrapper::get_length)
                            .addProperty("loop",                &SoundWrapper::get_loop,                &SoundWrapper::set_loop)
                            .addProperty("pitch",               &SoundWrapper::get_pitch,               &SoundWrapper::set_pitch)
                            .addProperty("volume",              &SoundWrapper::get_volume,              &SoundWrapper::set_volume)
                            .addProperty("position",            &SoundWrapper::get_position,            &SoundWrapper::set_position)
                            .addCFunction("__len",              &SoundWrapper::__len)
                            .addCFunction("play",               &SoundWrapper::play)
                            .addCFunction("stop",               &SoundWrapper::stop)
                            .addCFunction("reset",              &SoundWrapper::reset)
                        .endClass()

                        .addCFunction("openSound", &audio_openSound)

                        .beginClass<SoundEffectWrapper>("SoundEffect")
                            .addProperty("volume",              &SoundEffectWrapper::get_volume,        &SoundEffectWrapper::set_volume)
                            .addCFunction("play",               &SoundEffectWrapper::play)
                            .addCFunction("stop",               &SoundEffectWrapper::stop)
                        .endClass()

                        .addCFunction("openSoundEffect", &audio_openSoundEffect)

                    .endNamespace();

                return true;
            }

        } // audio_module
    } // namespace script
} // namespace rpgss
