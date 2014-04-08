#ifndef RPGSS_SCRIPT_AUDIO_MODULE_SOUNDEFFECTWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_AUDIO_MODULE_SOUNDEFFECTWRAPPER_HPP_INCLUDED

#include "../../audio/SoundEffect.hpp"
#include "../lua_include.hpp"



namespace rpgss {
    namespace script {
        namespace audio_module {

            class SoundEffectWrapper {
            public:
                static void Push(lua_State* L, audio::SoundEffect* soundeffect);
                static bool Is(lua_State* L, int index);
                static audio::SoundEffect* Get(lua_State* L, int index);
                static audio::SoundEffect* GetOpt(lua_State* L, int index);

                explicit SoundEffectWrapper(audio::SoundEffect* ptr);

                float get_volume() const;
                void set_volume(float volume);
                int play(lua_State* L);
                int stop(lua_State* L);

            private:
                audio::SoundEffect::Ptr This;
            };

        } // audio_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_AUDIO_MODULE_SOUNDEFFECTWRAPPER_HPP_INCLUDED
