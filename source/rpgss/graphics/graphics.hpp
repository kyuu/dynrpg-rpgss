#ifndef RPGSS_GRAPHICS_GRAPHICS_HPP_INCLUDED
#define RPGSS_GRAPHICS_GRAPHICS_HPP_INCLUDED

#include "../io/File.hpp"
#include "Font.hpp"
#include "WindowSkin.hpp"
#include "Image.hpp"


namespace rpgss {
    namespace graphics {

        bool InitGraphicsSubsystem();
        void DeinitGraphicsSubsystem();

        Image::Ptr ReadImage(const std::string& filename);
        Image::Ptr ReadImage(io::File* istream);

        bool WriteImage(const Image* image, const std::string& filename);
        bool WriteImage(const Image* image, io::File* ostream);

    } // namespace graphics
} // namespace rpgss


#endif // RPGSS_GRAPHICS_GRAPHICS_HPP_INCLUDED
