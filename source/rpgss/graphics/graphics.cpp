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

#include <cstring>

#include <memory>

#include <corona.h>

#include "../debug/debug.hpp"
#include "../io/io.hpp"
#include "../common/cpuinfo.hpp"
#include "graphics.hpp"


namespace rpgss {
    namespace graphics {

        //-----------------------------------------------------------------
        struct CoronaFileAdapter : public corona::DLLImplementation<corona::File>
        {
            CoronaFileAdapter(io::File* stream) : _stream(stream) {
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
                case corona::File::BEGIN:   return _stream->seek(pos, io::File::Begin);
                case corona::File::CURRENT: return _stream->seek(pos, io::File::Current);
                case corona::File::END:     return _stream->seek(pos, io::File::End);
                default:
                    return false;
                }
            }

            int COR_CALL tell() {
                return _stream->tell();
            }

        private:
            io::File::Ptr _stream;
        };

        //-----------------------------------------------------------------
        bool InitGraphicsSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::graphics::InitGraphicsSubsystem()")

            debug::Log() << "Initializing graphics subsystem...";

            debug::Log() << "CPU supports MMX:  " << (CpuSupportsMmx()  ? "yes" : "no");
            debug::Log() << "CPU supports SSE:  " << (CpuSupportsSse()  ? "yes" : "no");
            debug::Log() << "CPU supports SSE2: " << (CpuSupportsSse2() ? "yes" : "no");
            debug::Log() << "CPU supports SSE3: " << (CpuSupportsSse3() ? "yes" : "no");

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
            io::File::Ptr file = io::OpenFile(filename);

            if (!file) {
                return 0;
            }

            return ReadImage(file);
        }

        //-----------------------------------------------------------------
        Image::Ptr ReadImage(io::File* istream)
        {
            CoronaFileAdapter fileAdapter(istream);
            std::auto_ptr<corona::Image> img(corona::OpenImage(&fileAdapter, corona::PF_R8G8B8A8));

            if (!img.get()) {
                return 0;
            }

            return Image::New(img->getWidth(), img->getHeight(), (const RGBA*)img->getPixels());
        }

        //-----------------------------------------------------------------
        bool WriteImage(const Image* image, const std::string& filename)
        {
            io::File::Ptr file = io::OpenFile(filename, io::File::Out);

            if (!file) {
                return false;
            }

            return WriteImage(image, file);
        }

        //-----------------------------------------------------------------
        bool WriteImage(const Image* image, io::File* ostream)
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
