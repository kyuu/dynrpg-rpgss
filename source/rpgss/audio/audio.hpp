#ifndef RPGSS_AUDIO_AUDIO_HPP_INCLUDED
#define RPGSS_AUDIO_AUDIO_HPP_INCLUDED

#include <string>

#include "../io/File.hpp"
#include "Sound.hpp"
#include "SoundEffect.hpp"


namespace rpgss {
    namespace audio {

        bool InitAudioSubsystem();
        void DeinitAudioSubsystem();

        Sound::Ptr OpenSound(const std::string& filename);
        Sound::Ptr OpenSound(io::File* istream);
        SoundEffect::Ptr OpenSoundEffect(const std::string& filename);
        SoundEffect::Ptr OpenSoundEffect(io::File* istream);

    } // namespace audio
} // namespace rpgss


#endif // RPGSS_AUDIO_AUDIO_HPP_INCLUDED
