#ifndef RPGSS_AUDIO_SOUNDEFFECT_HPP_INCLUDED
#define RPGSS_AUDIO_SOUNDEFFECT_HPP_INCLUDED

#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"


namespace rpgss {
    namespace audio {

        class SoundEffect : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<SoundEffect> Ptr;

        public:
            virtual void play() = 0;
            virtual void stop() = 0;
            virtual float getVolume() const = 0;
            virtual void setVolume(float volume) = 0;

        protected:
            virtual ~SoundEffect() { }
        };

    } // namespace audio
} // namespace rpgss


#endif // RPGSS_AUDIO_SOUNDEFFECT_HPP_INCLUDED
