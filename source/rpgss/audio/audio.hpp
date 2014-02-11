#ifndef RPGSS_AUDIO_AUDIO_HPP_INCLUDED
#define RPGSS_AUDIO_AUDIO_HPP_INCLUDED

#include <string>

#include "../io/Stream.hpp"
#include "Sound.hpp"
#include "SoundEffect.hpp"


namespace rpgss {
    namespace audio {

        bool InitAudioSubsystem();
        void DeinitAudioSubsystem();

        Sound::Ptr OpenSound(const std::string& filename);
        Sound::Ptr OpenSound(io::Stream* istream);
        SoundEffect::Ptr OpenSoundEffect(const std::string& filename);
        SoundEffect::Ptr OpenSoundEffect(io::Stream* istream);

    } // namespace audio
} // namespace rpgss


#endif // RPGSS_AUDIO_AUDIO_HPP_INCLUDED
