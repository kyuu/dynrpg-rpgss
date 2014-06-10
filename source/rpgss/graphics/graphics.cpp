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

#include <azura.hpp>

#include "../debug/debug.hpp"
#include "../io/io.hpp"
#include "../common/cpuinfo.hpp"
#include "graphics.hpp"


namespace rpgss {
    namespace graphics {

        //-----------------------------------------------------------------
        class AzuraFileAdapter : public azura::File {
        public:
            explicit AzuraFileAdapter(io::File* file) : _file(file) {
            }

            ~AzuraFileAdapter() {
            }

            bool isOpen() const {
                return _file->isOpen();
            }

            void close() {
                _file->close();
            }

            bool eof() const {
                return _file->eof();
            }

            bool seek(int offset, azura::File::SeekMode mode) {
                switch (mode) {
                    case azura::File::Begin:
                        return _file->seek(offset, io::File::Begin);
                    case azura::File::Current:
                        return _file->seek(offset, io::File::Current);
                    case azura::File::End:
                        return _file->seek(offset, io::File::End);
                    default:
                        return false;
                }
            }

            int tell() {
                return _file->tell();
            }

            int read(azura::u8* buffer, int size) {
                return _file->read((u8*)buffer, size);
            }

            int write(const azura::u8* buffer, int size) {
                return _file->write((const u8*)buffer, size);
            }

            bool flush() {
                return _file->flush();
            }

        private:
            io::File::Ptr _file;
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

            debug::Log() << "Linked Azura version is " << azura::GetVersionString();

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
        Image::Ptr ReadImage(io::File* file)
        {
            azura::File::Ptr file_adapter = new AzuraFileAdapter(file);
            azura::Image::Ptr image = azura::ReadImage(file_adapter, azura::FileFormat::AutoDetect, azura::PixelFormat::RGBA);

            if (!image) {
                return 0;
            }

            return Image::New(image->getWidth(), image->getHeight(), (const RGBA*)image->getPixels());
        }

        //-----------------------------------------------------------------
        bool WriteImage(const Image* image, const std::string& filename, bool palletize, i32 mask)
        {
            io::File::Ptr file = io::OpenFile(filename, io::File::Out);

            if (!file) {
                return false;
            }

            return WriteImage(image, file, palletize, mask);
        }

        //-----------------------------------------------------------------
        bool WriteImage(const Image* image, io::File* file, bool palletize, i32 mask)
        {
            azura::File::Ptr file_adapter = new AzuraFileAdapter(file);
            azura::Image::Ptr azura_image = azura::CreateImage(image->getWidth(), image->getHeight(), azura::PixelFormat::RGBA);

            if (!azura_image) {
                return false;
            }

            std::memcpy(azura_image->getPixels(), image->getPixels(), image->getSizeInBytes());

            if (palletize) {
                azura_image = azura_image->convert(azura::PixelFormat::RGB_P8);
                if (!azura_image) {
                    return false;
                }

                if (mask >= 0) {
                    azura::RGB mask_color = {
                        (azura::u8)(mask >> 16),
                        (azura::u8)(mask >> 8),
                        (azura::u8)mask
                    };
                    azura::RGB* palette = azura_image->getPalette();

                    // see if mask color exists in the palette
                    int mask_index = -1;
                    for (int i = 0; i < 256; i++) {
                        azura::RGB* color = palette + i;
                        if (color->red   == mask_color.red   &&
                            color->green == mask_color.green &&
                            color->blue  == mask_color.blue)
                        {
                            mask_index = i;
                            break;
                        }
                    }

                    // if mask color exists in the palette (and isn't already
                    // at index 0), we move it to index 0
                    if (mask_index > 0) {
                        // swap palette entries
                        palette[mask_index] = palette[0];
                        palette[0] = mask_color;

                        // update pixels
                        azura::u8* pixel_ptr = azura_image->getPixels();
                        int num_pixels = azura_image->getWidth() * azura_image->getHeight();
                        for (int i = 0; i < num_pixels; i++, pixel_ptr++) {
                            int index = *pixel_ptr;
                            if (index == 0) {
                                *pixel_ptr = mask_index;
                            } else if (index == mask_index) {
                                *pixel_ptr = 0;
                            }
                        }
                    }
                }
            }

            return azura::WriteImage(azura_image, file_adapter, azura::FileFormat::PNG);
        }

    } // namespace graphics
} // namespace rpgss
