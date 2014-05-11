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

#include <cassert>

#include <physfs.h>

#include "../debug/debug.hpp"
#include "io.hpp"


namespace rpgss {
    namespace io {

        //--------------------------------------------------------------
        class FileImpl : public File {
        public:
            explicit FileImpl(PHYSFS_File* pf)
                : _pf(pf)
                , _error(false)
            {
            }

            ~FileImpl() {
                close();
            }

            bool isOpen() const {
                return _pf != 0;
            }

            int getSize() const {
                if (isOpen()) {
                    return PHYSFS_fileLength(_pf);
                }
                return -1;
            }

            void close() {
                if (isOpen()) {
                    PHYSFS_close(_pf);
                    _pf = 0;
                    _error = false;
                }
            }

            bool eof() const {
                if (isOpen()) {
                    return PHYSFS_eof(_pf) != 0;
                } else {
                    return true;
                }
            }

            bool error() const {
                return _error;
            }

            bool good() const {
                return !eof() && !error();
            }

            void clearerr() {
                // clear eof flag
                PHYSFS_sint64 cur_pos = PHYSFS_tell(_pf);
                if (cur_pos >= 0) {
                    PHYSFS_seek(_pf, (PHYSFS_uint64)cur_pos);
                }
                // clear error flag
                _error = false;
            }

            bool seek(int offset, SeekMode mode) {
                if (isOpen()) {
                    switch (mode) {
                    case Begin: {
                        if (offset < 0) {
                            _error = true;
                            return false;
                        }
                        if (PHYSFS_seek(_pf, (PHYSFS_uint64)offset) == 0) {
                            _error = true;
                            return false;
                        }
                        return true;
                    }
                    case Current: {
                        PHYSFS_sint64 cur_pos = PHYSFS_tell(_pf);
                        if (cur_pos < 0 || cur_pos + offset < 0) {
                            _error = true;
                            return false;
                        }
                        if (PHYSFS_seek(_pf, (PHYSFS_uint64)(cur_pos + offset)) == 0) {
                            _error = true;
                            return false;
                        }
                        return true;
                    }
                    case End: {
                        PHYSFS_sint64 file_len = PHYSFS_fileLength(_pf);
                        if (file_len < 0 || offset > 0 || file_len + offset < 0) {
                            _error = true;
                            return false;
                        }
                        if (PHYSFS_seek(_pf, (PHYSFS_uint64)(file_len + offset)) == 0) {
                            _error = true;
                            return false;
                        }
                        return true;
                    }
                    default:
                        _error = true;
                        return false;
                    }
                } else {
                    _error = true;
                    return false;
                }
            }

            int tell() {
                if (isOpen()) {
                    PHYSFS_sint64 cur_pos = PHYSFS_tell(_pf);
                    if (cur_pos < 0) {
                        _error = true;
                        return false;
                    }
                    return (int)cur_pos;
                } else {
                    _error = true;
                    return -1;
                }
            }

            int read(u8* buffer, int size) {
                if (isOpen()) {
                    PHYSFS_sint64 retval = PHYSFS_read(_pf, buffer, 1, size);
                    if (retval < 0) {
                        _error = true;
                        return 0;
                    }
                    return retval;
                } else {
                    _error = true;
                    return 0;
                }
            }

            int write(const u8* buffer, int size) {
                if (isOpen()) {
                    PHYSFS_sint64 retval = PHYSFS_write(_pf, buffer, 1, size);
                    if (retval < 0) {
                        _error = true;
                        return 0;
                    }
                    return retval;
                } else {
                    _error = true;
                    return 0;
                }
            }

            bool flush() {
                if (isOpen()) {
                    return PHYSFS_flush(_pf) != 0;
                } else {
                    _error = true;
                    return false;
                }
            }

        private:
            PHYSFS_File* _pf;
            bool _error;
        };

        //--------------------------------------------------------------
        bool InitIoSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::io::InitIoSubsystem()")

            assert(PHYSFS_isInit() == 0);

            if (PHYSFS_isInit() == 0) {
                debug::Log() << "Initializing i/o subsystem...";

                PHYSFS_Version linked;
                PHYSFS_getLinkedVersion(&linked);
                debug::Log() << "Linked PhysFS version is " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch;

                debug::Log() << "Initializing PhysFS library...";
                if (PHYSFS_init(0) == 0) {
                    debug::Log() << "Initializing PhysFS library failed";
                    return false;
                }

                debug::Log() << "Base directory is '" << PHYSFS_getBaseDir() << "'";

                debug::Log() << "Mounting base directory...";
                if (PHYSFS_mount(PHYSFS_getBaseDir(), 0, 1) == 0) {
                    debug::Log() << "Mounting base directory failed";
                    return false;
                }

                debug::Log() << "Setting write directory...";
                if (PHYSFS_setWriteDir(PHYSFS_getBaseDir()) == 0) {
                    debug::Log() << "Setting write directory failed";
                    return false;
                }

                // automatically mount all archives in ~/Data
                std::vector<std::string> fileList;
                Enumerate("Data", fileList); // if fails, treat as non-fatal error
                debug::Log() << "Found " << fileList.size() << " file(s) in '~/Data'";
                for (size_t i = 0; i < fileList.size(); i++) {
                    debug::Log() << "Mounting '" << fileList[i] << "'...";
                    std::string file_path = "Data/" + fileList[i];
                    if (PHYSFS_mount(file_path.c_str(), 0, 1) == 0) {
                        debug::Log() << "Mounting '" << fileList[i] << "' failed: " << PHYSFS_getLastError();
                        return false;
                    }
                }
            }

            return true;
        }

        //--------------------------------------------------------------
        void DeinitIoSubsystem()
        {
            RPGSS_DEBUG_GUARD("rpgss::io::DeinitIoSubsystem()")

            debug::Log() << "Deinitializing PhysFS library...";
            if (PHYSFS_isInit() != 0) {
                if (PHYSFS_deinit() == 0) {
                    debug::Log() << "Deinitializing PhysFS library failed";
                }
            }
        }

        //--------------------------------------------------------------
        File::Ptr OpenFile(const std::string& filename, File::OpenMode mode)
        {
            PHYSFS_File* pf = 0;

            switch (mode) {
            case File::In:     pf = PHYSFS_openRead(filename.c_str());   break;
            case File::Out:    pf = PHYSFS_openWrite(filename.c_str());  break;
            case File::Append: pf = PHYSFS_openAppend(filename.c_str()); break;
            }

            if (pf) {
                return new FileImpl(pf);
            }

            return 0;
        }

        //--------------------------------------------------------------
        bool MakeDirectory(const std::string& dirname)
        {
            return PHYSFS_mkdir(dirname.c_str()) != 0;
        }

        //--------------------------------------------------------------
        bool Remove(const std::string& filename)
        {
            return PHYSFS_delete(filename.c_str()) != 0;
        }

        //--------------------------------------------------------------
        bool Enumerate(const std::string& dirname, std::vector<std::string>& filelist)
        {
            char** fl = PHYSFS_enumerateFiles(dirname.c_str());
            if (fl) {
                for (char** i = fl; *i != 0; ++i) {
                    filelist.push_back(*i);
                }
                PHYSFS_freeList(fl);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------
        bool Exists(const std::string& filename)
        {
            return PHYSFS_exists(filename.c_str()) != 0;
        }

        //--------------------------------------------------------------
        int LastWriteTime(const std::string& filename)
        {
            return (int)PHYSFS_getLastModTime(filename.c_str());
        }

        //--------------------------------------------------------------
        bool IsDirectory(const std::string& filename)
        {
            return PHYSFS_isDirectory(filename.c_str()) != 0;
        }

        //--------------------------------------------------------------
        bool IsFile(const std::string& filename)
        {
            return Exists(filename) && !IsDirectory(filename);
        }

        //--------------------------------------------------------------
        bool Mount(const std::string& filename)
        {
            return PHYSFS_mount(filename.c_str(), 0, 1) != 0;
        }

        //--------------------------------------------------------------
        bool Unmount(const std::string& filename)
        {
            return PHYSFS_removeFromSearchPath(filename.c_str()) != 0;
        }

    } // namespace io
} // namespace rpgss
