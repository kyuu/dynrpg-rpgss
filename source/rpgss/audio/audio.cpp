#include <cstring> // needed by <audiere.h>
#include <cassert>

#include <vector>
#include <string>

#include <audiere.h>

#include "../debug/debug.hpp"
#include "../io/io.hpp"
#include "audio.hpp"


namespace rpgss {
    namespace audio {

        //-----------------------------------------------------------------
        class AudiereFileAdapter : public audiere::RefImplementation<audiere::File> {
        public:
            explicit AudiereFileAdapter(io::File* istream) : _istream(istream) {
            }

            ~AudiereFileAdapter() {
            }

            ADR_METHOD(int) read(void* buffer, int size) {
                return _istream->read((u8*)buffer, size);
            }

            ADR_METHOD(bool) seek(int pos, audiere::File::SeekMode mode) {
                switch (mode) {
                case audiere::File::BEGIN:   return _istream->seek(pos, io::File::Begin);
                case audiere::File::CURRENT: return _istream->seek(pos, io::File::Current);
                case audiere::File::END:     return _istream->seek(pos, io::File::End);
                default:
                    return false;
                }
            }

            ADR_METHOD(int) tell() {
                return _istream->tell();
            }

        private:
            io::File::Ptr _istream;
        };

        //-----------------------------------------------------------------
        class SoundImpl : public Sound {
        public:
            SoundImpl(audiere::OutputStream* ass) : _ass(ass) {
                _ass->ref();
            }

            ~SoundImpl() {
                _ass->unref();
            }

            void play() {
                _ass->play();
            }

            void stop() {
                _ass->stop();
            }

            void reset() {
                _ass->reset();
            }

            bool isPlaying() const {
                return _ass->isPlaying();
            }

            bool isSeekable() const {
                return _ass->isSeekable();
            }

            int getLength() const {
                return _ass->getLength();
            }

            bool getRepeat() const {
                return _ass->getRepeat();
            }

            void setRepeat(bool repeat) {
                _ass->setRepeat(repeat);
            }

            float getVolume() const {
                return _ass->getVolume();
            }

            void setVolume(float volume) {
                _ass->setVolume(volume);
            }

            int getPosition() const {
                return _ass->getPosition();
            }

            void setPosition(int position) {
                _ass->setPosition(position);
            }

        private:
            audiere::OutputStream* _ass;
        };

        //-----------------------------------------------------------------
        class SoundEffectImpl : public SoundEffect {
        public:
            SoundEffectImpl(audiere::SoundEffect* ase) : _ase(ase) {
                _ase->ref();
            }

            ~SoundEffectImpl() {
                _ase->unref();
            }

            void play() {
                _ase->play();
            }

            void stop() {
                _ase->stop();
            }

            float getVolume() const {
                return _ase->getVolume();
            }

            void setVolume(float volume) {
                _ase->setVolume(volume);
            }

        private:
            audiere::SoundEffect* _ase;
        };

        //-----------------------------------------------------------------
        // globals
        static audiere::AudioDevice* g_AudiereAudioDevice = 0;

        //-----------------------------------------------------------------
        bool InitAudioSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::audio::InitAudioSubsystem()")

            assert(g_AudiereAudioDevice == 0);

            if (g_AudiereAudioDevice == 0) {
                debug::Log() << "Initializing audio subsystem...";

                debug::Log() << "Linked Audiere version is " << audiere::GetVersion();

                // see if there are any audio devices available
                debug::Log() << "Enumerating supported audio devices...";
                std::vector<audiere::AudioDeviceDesc> devices;
                audiere::GetSupportedAudioDevices(devices);
                if (devices.empty()) {
                    debug::Log() << "No supported audio devices found";
                    return false;
                } else {
                    for (size_t i = 0; i < devices.size(); i++) {
                        debug::Log() << "Supported audio device: " << devices[i].name << " (" << devices[i].description << ")";
                    }
                }

                // see if audiere supports any audio file format
                debug::Log() << "Enumerating supported audio file formats...";
                std::vector<audiere::FileFormatDesc> formats;
                audiere::GetSupportedFileFormats(formats);
                if (formats.empty()) {
                    debug::Log() << "No supported audio file formats found";
                    return false;
                } else {
                    for (size_t i = 0; i < formats.size(); i++) {
                        std::ostringstream extensions;
                        for (size_t j = 0; j < formats[i].extensions.size(); j++) {
                            extensions << formats[i].extensions[j];
                            if (j != formats[i].extensions.size()-1) {
                                extensions << ", ";
                            }
                        }
                        debug::Log() << "Supported audio file format: " << extensions.str() << " (" << formats[i].description << ")";
                    }
                }

                // open default audio device
                debug::Log() << "Opening default audio device...";
                g_AudiereAudioDevice = audiere::OpenDevice(0);
                if (!g_AudiereAudioDevice) {
                    debug::Log() << "Opening default audio device failed";
                    return false;
                }
                g_AudiereAudioDevice->ref();
            }

            return true;
        }

        //-----------------------------------------------------------------
        void DeinitAudioSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::audio::DeinitAudioSubsystem()")

            if (g_AudiereAudioDevice) {
                g_AudiereAudioDevice->unref();
                g_AudiereAudioDevice = 0;
            }
        }

        //-----------------------------------------------------------------
        Sound::Ptr OpenSound(const std::string& filename)
        {
            io::File::Ptr file = io::OpenFile(filename);

            if (!file) {
                return 0;
            }

            return OpenSound(file);
        }

        //-----------------------------------------------------------------
        Sound::Ptr OpenSound(io::File* istream)
        {
            audiere::FilePtr audiereFile(new AudiereFileAdapter(istream));
            audiere::OutputStream* as = audiere::OpenSound(g_AudiereAudioDevice, audiereFile, true);

            if (!as) {
                return 0;
            }

            return new SoundImpl(as);
        }

        //-----------------------------------------------------------------
        SoundEffect::Ptr OpenSoundEffect(const std::string& filename)
        {
            io::File::Ptr file = io::OpenFile(filename);

            if (!file) {
                return 0;
            }

            return OpenSoundEffect(file);
        }

        //-----------------------------------------------------------------
        SoundEffect::Ptr OpenSoundEffect(io::File* istream)
        {
            audiere::FilePtr audiereFile(new AudiereFileAdapter(istream));
            audiere::SoundEffect* ase = audiere::OpenSoundEffect(g_AudiereAudioDevice, audiereFile, audiere::MULTIPLE);

            if (!ase) {
                return 0;
            }

            return new SoundEffectImpl(ase);
        }

    } // namespace audio
} // namespace rpgss
