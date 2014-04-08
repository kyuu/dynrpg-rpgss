#ifndef RPGSS_SCRIPT_AUDIO_MODULE_AUDIO_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_AUDIO_MODULE_AUDIO_MODULE_HPP_INCLUDED

#include "../../audio/audio.hpp"
#include "../lua_include.hpp"
#include "SoundWrapper.hpp"
#include "SoundEffectWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace audio_module {

            bool RegisterAudioModule(lua_State* L);

        } // audio_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_AUDIO_MODULE_AUDIO_MODULE_HPP_INCLUDED
