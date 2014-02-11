#include <cstring>

#include <memory>

#include <corona.h>

#include "../debug/debug.hpp"
#include "../io/io.hpp"
#include "cpuinfo.hpp"
#include "graphics.hpp"


namespace rpgss {
    namespace graphics {

        //-----------------------------------------------------------------
        struct CoronaFileAdapter : public corona::DLLImplementation<corona::File>
        {
            CoronaFileAdapter(io::Stream* stream) : _stream(stream) {
            }

            ~CoronaFileAdapter() {
            }

            int COR_CALL read(void* buffer, int size) {
                return _stream->read((u8*)buffer, size);
            }

            int COR_CALL write(const void* buffer, int size) {
                return _stream->write((const u8*)buffer, size);
            }

            bool COR_CALL seek(int pos, corona::File::SeekMode mode) {
                switch (mode) {
                case corona::File::BEGIN:   return _stream->seek(pos, io::Stream::Begin);
                case corona::File::CURRENT: return _stream->seek(pos, io::Stream::Current);
                case corona::File::END:     return _stream->seek(pos, io::Stream::End);
                default:
                    return false;
                }
            }

            int COR_CALL tell() {
                return _stream->tell();
            }

        private:
            io::Stream::Ptr _stream;
        };

        //-----------------------------------------------------------------
        bool InitGraphicsSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::graphics::InitGraphicsSubsystem()")

            debug::Log() << "Initializing graphics subsystem...";

            debug::Log() << "CPU supports MMX:  " << (CPUSupportsMMX()  ? "yes" : "no");
            debug::Log() << "CPU supports SSE:  " << (CPUSupportsSSE()  ? "yes" : "no");
            debug::Log() << "CPU supports SSE2: " << (CPUSupportsSSE2() ? "yes" : "no");
            debug::Log() << "CPU supports SSE3: " << (CPUSupportsSSE3() ? "yes" : "no");

            debug::Log() << "Linked Corona version is " << corona::GetVersion();

            debug::Log() << "Enumerating supported image read formats...";
            corona::FileFormatDesc** read_formats = corona::GetSupportedReadFormats();
            for (int i = 0; read_formats[i] != 0; i++) {
                std::string extensions;
                for (size_t j = 0; j < read_formats[i]->getExtensionCount(); j++) {
                    extensions += read_formats[i]->getExtension(j);
                    if (j+1 < read_formats[i]->getExtensionCount()) {
                        extensions += ", ";
                    }
                }
                debug::Log() << "Supported image read format: " << extensions << " (" << read_formats[i]->getDescription() << ")";
            }

            debug::Log() << "Enumerating supported image write formats...";
            corona::FileFormatDesc** write_formats = corona::GetSupportedWriteFormats();
            for (int i = 0; write_formats[i] != 0; i++) {
                std::string extensions;
                for (size_t j = 0; j < write_formats[i]->getExtensionCount(); j++) {
                    extensions += write_formats[i]->getExtension(j);
                    if (j+1 < write_formats[i]->getExtensionCount()) {
                        extensions += ", ";
                    }
                }
                debug::Log() << "Supported image write format: " << extensions << " (" << write_formats[i]->getDescription() << ")";
            }

            return true;
        }

        //-----------------------------------------------------------------
        void DeinitGraphicsSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::graphics::DeinitGraphicsSubsystem()")
        }

        //-----------------------------------------------------------------
        Image::Ptr ReadImage(const std::string& filename)
        {
            io::FileStream::Ptr file = io::OpenFile(filename);

            if (!file) {
                return 0;
            }

            return ReadImage(file);
        }

        //-----------------------------------------------------------------
        Image::Ptr ReadImage(io::Stream* istream)
        {
            CoronaFileAdapter fileAdapter(istream);
            std::auto_ptr<corona::Image> img(corona::OpenImage(&fileAdapter, corona::PF_R8G8B8A8));

            if (!img.get()) {
                return 0;
            }

            return Image::New(img->getWidth(), img->getHeight(), (const RGBA*)img->getPixels());
        }

        //-----------------------------------------------------------------
        bool WriteImage(Image* image, const std::string& filename)
        {
            io::FileStream::Ptr file = io::OpenFile(filename, io::Stream::Out);

            if (!file) {
                return false;
            }

            return WriteImage(image, file);
        }

        //-----------------------------------------------------------------
        bool WriteImage(Image* image, io::Stream* ostream)
        {
            std::auto_ptr<corona::Image> img(corona::CreateImage(image->getWidth(), image->getHeight(), corona::PF_R8G8B8A8));

            if (!img.get()) {
                return false;
            }

            std::memcpy(img->getPixels(), image->getPixels(), image->getSizeInBytes());

            CoronaFileAdapter fileAdapter(ostream);
            return corona::SaveImage(&fileAdapter, corona::FF_PNG, img.get());
        }

    } // namespace graphics
} // namespace rpgss
