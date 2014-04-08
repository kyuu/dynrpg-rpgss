#include "../core_module/core_module.hpp"
#include "util_module.hpp"


namespace rpgss {
    namespace script {
        namespace util_module {

            //---------------------------------------------------------
            int util_compress(lua_State* L)
            {
                core::ByteArray* data = core_module::ByteArrayWrapper::Get(L, 1);
                if (data->getSize() == 0) {
                    return luaL_error(L, "empty data");
                }
                io::MemoryFile::Ptr output_stream = io::MemoryFile::New();
                if (util::Compress(data->getBuffer(), data->getSize(), output_stream)) {
                    core::ByteArray::Ptr compressed_data = core::ByteArray::New(output_stream->getBuffer(), output_stream->getSize());
                    core_module::ByteArrayWrapper::Push(L, compressed_data);
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            int util_decompress(lua_State* L)
            {
                core::ByteArray* compressed_data = core_module::ByteArrayWrapper::Get(L, 1);
                if (compressed_data->getSize() == 0) {
                    return luaL_error(L, "empty compressed data");
                }
                io::MemoryFile::Ptr output_stream = io::MemoryFile::New();
                if (util::Decompress(compressed_data->getBuffer(), compressed_data->getSize(), output_stream)) {
                    core::ByteArray::Ptr data = core::ByteArray::New(output_stream->getBuffer(), output_stream->getSize());
                    core_module::ByteArrayWrapper::Push(L, data);
                } else {
                    lua_pushnil(L);
                }
                return 1;
            }

            //---------------------------------------------------------
            bool RegisterUtilModule(lua_State* L)
            {
                luabridge::getGlobalNamespace(L)
                    .beginNamespace("util")

                        .addCFunction("compress",   &util_compress)
                        .addCFunction("decompress", &util_decompress)

                    .endNamespace();

                return true;
            }

        } // util_module
    } // namespace script
} // namespace rpgss
