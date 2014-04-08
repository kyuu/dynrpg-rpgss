#ifndef RPGSS_IO_IO_HPP_INCLUDED
#define RPGSS_IO_IO_HPP_INCLUDED

#include <vector>
#include <string>
#include "File.hpp"
#include "MemoryFile.hpp"
#include "Reader.hpp"
#include "Writer.hpp"


namespace rpgss {
    namespace io {

        bool InitIoSubsystem();
        void DeinitIoSubsystem();

        File::Ptr OpenFile(const std::string& filename, File::OpenMode mode = File::In);
        bool MakeDirectory(const std::string& dirname);
        bool Remove(const std::string& filename);
        bool Enumerate(const std::string& dirname, std::vector<std::string>& filelist);
        bool Exists(const std::string& filename);
        int  LastWriteTime(const std::string& filename);
        bool IsDirectory(const std::string& filename);
        bool IsFile(const std::string& filename);
        bool Mount(const std::string& filename);
        bool Unmount(const std::string& filename);

    } // namespace io
} // namespace rpgss


#endif // RPGSS_IO_IO_HPP_INCLUDED
