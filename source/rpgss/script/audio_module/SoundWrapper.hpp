#ifndef RPGSS_SCRIPT_AUDIO_MODULE_SOUNDWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_AUDIO_MODULE_SOUNDWRAPPER_HPP_INCLUDED

#include "../../audio/Sound.hpp"
#include "../lua_include.hpp"


namespace rpgss {
    namespace script {
        namespace audio_module {

            class SoundWrapper {
            public:
                static void Push(lua_State* L, audio::Sound* sound);
                static bool Is(lua_State* L, int index);
                static audio::Sound* Get(lua_State* L, int index);
                static audio::Sound* GetOpt(lua_State* L, int index);

                explicit SoundWrapper(audio::Sound* ptr);

                bool get_isPlaying() const;
                bool get_isSeekable() const;
                int get_length() const;
                bool get_repeat() const;
                void set_repeat(bool repeat);
                float get_volume() const;
                void set_volume(float volume);
                int get_position() const;
                void set_position(int position);
                int __len(lua_State* L);
                int play(lua_State* L);
                int stop(lua_State* L);
                int reset(lua_State* L);

            private:
                audio::Sound::Ptr This;
            };

        } // audio_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_AUDIO_MODULE_SOUNDWRAPPER_HPP_INCLUDED
