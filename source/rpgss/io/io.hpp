#ifndef RPGSS_IO_IO_HPP_INCLUDED
#define RPGSS_IO_IO_HPP_INCLUDED

#include <vector>
#include <string>
#include "FileStream.hpp"
#include "MemoryStream.hpp"
#include "BinaryReader.hpp"
#include "BinaryWriter.hpp"
#include "Compressor.hpp"
#include "Decompressor.hpp"


namespace rpgss {
    namespace io {

        bool InitIoSubsystem();
        void DeinitIoSubsystem();

        FileStream::Ptr OpenFile(const std::string& filename, Stream::OpenMode mode = Stream::In);
        bool MakeDirectory(const std::string& dirname);
        bool Remove(const std::string& filename);
        bool Enumerate(const std::string& dirname, std::vector<std::string>& filelist);
        bool Exists(const std::string& filename);
        int  LastWriteTime(const std::string& filename);
        bool IsDirectory(const std::string& filename);
        bool IsFile(const std::string& filename);
        bool Mount(const std::string& filename);
        bool Unmount(const std::string& filename);

        bool Compress(const u8* buffer, int size, Stream* ostream);
        bool Decompress(const u8* buffer, int size, Stream* ostream);

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_IO_HPP_INCLUDED
