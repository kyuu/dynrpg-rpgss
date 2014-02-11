#ifndef RPGSS_AUDIO_SOUND_HPP_INCLUDED
#define RPGSS_AUDIO_SOUND_HPP_INCLUDED

#include "../common/RefCountedObject.hpp"
#include "../common/RefCountedObjectPtr.hpp"


namespace rpgss {
    namespace audio {

        class Sound : public RefCountedObject {
        public:
            typedef RefCountedObjectPtr<Sound> Ptr;

        public:
            virtual void play() = 0;
            virtual void stop() = 0;
            virtual void reset() = 0;
            virtual bool isPlaying() const = 0;
            virtual bool isSeekable() const = 0;
            virtual int  getLength() const = 0;
            virtual bool getRepeat() const = 0;
            virtual void setRepeat(bool repeat) = 0;
            virtual float getVolume() const = 0;
            virtual void setVolume(float volume) = 0;
            virtual int  getPosition() const = 0;
            virtual void setPosition(int position) = 0;

        protected:
            virtual ~Sound() { }
        };

    } // namespace audio
} // namespace rpgss


#endif // RPGSS_AUDIO_SOUND_HPP_INCLUDED
