#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#include <emmintrin.h>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../debug/debug.hpp"
#include "../core/ByteArray.hpp"
#include "../io/io.hpp"
#include "../timer/timer.hpp"
#include "../audio/audio.hpp"
#include "../graphics/graphics.hpp"
#include "../graphics/primitives.hpp"
#include "../graphics/cpuinfo.hpp"
#include "../input/input.hpp"
#include "../Context.hpp"
#include "rpg.hpp"

#define RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT   999999
#define RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT 999999


// register rpgss::RefCountedObjectPtr as container in luabridge
namespace luabridge {

    // forward declaration
    template <class T>
    struct ContainerTraits;

    template <class T>
    struct ContainerTraits <rpgss::RefCountedObjectPtr <T> >
    {
        typedef T Type;

        static T* get (rpgss::RefCountedObjectPtr <T> const& c)
        {
            return c.get ();
        }
    };

} // namespace luabridge

// Cherry's stuff not yet in his SDK
namespace RPG {

    //---------------------------------------------------------
    int getLowerLayerTileId(RPG::Map* map, int x, int y)
    {
        int ret;
        asm volatile("call *%%esi" : "=a" (ret), "=d" (RPG::_edx), "=c" (RPG::_ecx) : "S" (0x4A80CC), "a" (map), "d" (x), "c" (y) : "cc", "memory");
        return ret;
    }

    //---------------------------------------------------------
    int getUpperLayerTileId(RPG::Map* map, int x, int y)
    {
        int ret;
        asm volatile("call *%%esi" : "=a" (ret), "=d" (RPG::_edx), "=c" (RPG::_ecx) : "S" (0x4A80F4), "a" (map), "d" (x), "c" (y) : "cc", "memory");
        return ret;
    }

    //---------------------------------------------------------
    int getTerrainId(RPG::Map* map, int tileId)
    {
        int ret;
        asm volatile("movl 20(%%eax), %%eax; call *%%esi" : "=a" (ret), "=d" (RPG::_edx) : "S" (0x47D038), "a" (map), "d" (tileId) : "cc", "memory");
        return ret;
    }

} // namespace RPG

namespace rpgss {
    namespace script {

        /**********************************************************
         *                     BYTE ARRAY CLASS
         **********************************************************/

        struct ByteArrayWrapper {
            ByteArray::Ptr This;

            //---------------------------------------------------------
            explicit ByteArrayWrapper(ByteArray* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static ByteArray* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "ByteArray expected, got nothing");
                    return 0;
                }
                ByteArrayWrapper* wrapper = luabridge::Stack<ByteArrayWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "ByteArray expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static ByteArray* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ByteArrayWrapper* wrapper = luabridge::Stack<ByteArrayWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            int get_size() const
            {
                return This->getSize();
            }

            //---------------------------------------------------------
            int __len(lua_State* L)
            {
                lua_pushnumber(L, This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            int __add(lua_State* L)
            {
                ByteArray* rhs = ByteArrayWrapper::Get(L, 2);
                luabridge::push(L, ByteArrayWrapper(This->concat(rhs)));
                return 1;
            }

            //---------------------------------------------------------
            int __concat(lua_State* L)
            {
                ByteArray* rhs = ByteArrayWrapper::Get(L, 2);
                luabridge::push(L, ByteArrayWrapper(This->concat(rhs)));
                return 1;
            }

            //---------------------------------------------------------
            int toString(lua_State* L)
            {
                lua_pushlstring(L, (const char*)This->getBuffer(), This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            int getByte(lua_State* L)
            {
                int index = luaL_checkint(L, 2);
                if (index < 1 || index > This->getSize()) {
                    throw "index out of bounds";
                }
                lua_pushnumber(L, This->at(index - 1));
                return 1;
            }

            //---------------------------------------------------------
            int setByte(lua_State* L)
            {
                int index = luaL_checkint(L, 2);
                int value = luaL_checkint(L, 3);
                if (index < 1 || index > This->getSize()) {
                    throw "index out of bounds";
                }
                This->at(index - 1) = value;
                return 0;
            }

            //---------------------------------------------------------
            int resize(lua_State* L)
            {
                int size = luaL_checkint(L, 2);
                int value = luaL_optint(L, 3, 0);
                if (size < 0) {
                    throw "invalid size";
                }
                This->resize(size, value);
                return 0;
            }

            //---------------------------------------------------------
            int memset(lua_State* L)
            {
                int value = luaL_checkint(L, 2);
                This->memset(value);
                return 0;
            }

            //---------------------------------------------------------
            int clear(lua_State* L)
            {
                This->clear();
                return 0;
            }
        };

        /**********************************************************
         *                       STREAM CLASS
         **********************************************************/

        //---------------------------------------------------------
        bool get_seek_mode_constant(const std::string& seek_mode_str, int& out_seek_mode)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("begin",   io::Stream::Begin  )
                ("current", io::Stream::Current)
                ("end",     io::Stream::End    );

            map_type::iterator mapped_value = map.find(seek_mode_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_seek_mode = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        struct StreamWrapper {
            io::Stream::Ptr This;

            //---------------------------------------------------------
            explicit StreamWrapper(io::Stream* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static io::Stream* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Stream expected, got nothing");
                    return 0;
                }
                StreamWrapper* wrapper = luabridge::Stack<StreamWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Stream expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static io::Stream* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                StreamWrapper* wrapper = luabridge::Stack<StreamWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            bool get_eof() const
            {
                return This->eof();
            }

            //---------------------------------------------------------
            bool get_error() const
            {
                return This->error();
            }

            //---------------------------------------------------------
            bool get_good() const
            {
                return This->good();
            }

            //---------------------------------------------------------
            int clearError(lua_State* L)
            {
                This->clearerr();
                return 0;
            }

            //---------------------------------------------------------
            int seek(lua_State* L)
            {
                int offset = luaL_checkint(L, 2);
                const char* seek_mode_str = luaL_optstring(L, 3, "begin");
                int seek_mode;
                if (!get_seek_mode_constant(seek_mode_str, seek_mode)) {
                    throw "invalid seek mode constant";
                }
                lua_pushboolean(L, This->seek(offset, (io::Stream::SeekMode)seek_mode));
                return 1;
            }

            //---------------------------------------------------------
            int tell(lua_State* L)
            {
                lua_pushnumber(L, This->tell());
                return 1;
            }

            //---------------------------------------------------------
            int read(lua_State* L)
            {
                int size = luaL_checkint(L, 2);
                if (size < 0) {
                    throw "invalid size";
                }
                ByteArray::Ptr bytearray = ByteArray::New(size);
                int nread = This->read(bytearray->getBuffer(), bytearray->getSize());
                if (nread != size) {
                    bytearray->resize(nread);
                }
                luabridge::push(L, ByteArrayWrapper(bytearray));
                return 1;
            }

            //---------------------------------------------------------
            int write(lua_State* L)
            {
                ByteArray* byte_array = ByteArrayWrapper::Get(L, 2);
                This->write(byte_array->getBuffer(), byte_array->getSize());
                return 0;
            }

            //---------------------------------------------------------
            int flush(lua_State* L)
            {
                lua_pushboolean(L, This->flush());
                return 1;
            }
        };

        /**********************************************************
         *                    FILE STREAM CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct FileStreamWrapper : public StreamWrapper {
            io::FileStream::Ptr This;

            //---------------------------------------------------------
            explicit FileStreamWrapper(io::FileStream* ptr)
                : StreamWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            static io::FileStream* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "FileStream expected, got nothing");
                    return 0;
                }
                FileStreamWrapper* wrapper = luabridge::Stack<FileStreamWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "FileStream expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static io::FileStream* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                FileStreamWrapper* wrapper = luabridge::Stack<FileStreamWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            bool get_isOpen() const
            {
                return This->isOpen();
            }

            //---------------------------------------------------------
            int get_size() const
            {
                return This->getSize();
            }

            //---------------------------------------------------------
            int __len(lua_State* L)
            {
                lua_pushnumber(L, This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            int close(lua_State* L)
            {
                This->close();
                return 0;
            }
        };

        /**********************************************************
         *                    MEMORY STREAM CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct MemoryStreamWrapper : public StreamWrapper {
            io::MemoryStream::Ptr This;

            //---------------------------------------------------------
            explicit MemoryStreamWrapper(io::MemoryStream* ptr)
                : StreamWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            static io::MemoryStream* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "MemoryStream expected, got nothing");
                    return 0;
                }
                MemoryStreamWrapper* wrapper = luabridge::Stack<MemoryStreamWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "MemoryStream expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static io::MemoryStream* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                MemoryStreamWrapper* wrapper = luabridge::Stack<MemoryStreamWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            bool get_size() const
            {
                return This->getSize();
            }

            //---------------------------------------------------------
            bool get_capacity() const
            {
                return This->getCapacity();
            }

            //---------------------------------------------------------
            int __len(lua_State* L)
            {
                lua_pushnumber(L, This->getSize());
                return 1;
            }

            //---------------------------------------------------------
            int copyBuffer(lua_State* L)
            {
                ByteArray::Ptr bytearray = ByteArray::New(This->getBuffer(), This->getSize());
                luabridge::push(L, ByteArrayWrapper(bytearray));
                return 1;
            }

            //---------------------------------------------------------
            int reserve(lua_State* L)
            {
                int capacity = luaL_checkint(L, 2);
                if (capacity < 0) {
                    throw "invalid capacity";
                }
                This->reserve(capacity);
                return 0;
            }

            //---------------------------------------------------------
            int clear(lua_State* L)
            {
                This->clear();
                return 0;
            }
        };

        /**********************************************************
         *                   BINARY READER CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct BinaryReaderWrapper {
            io::BinaryReader::Ptr This;

            //---------------------------------------------------------
            explicit BinaryReaderWrapper(io::BinaryReader* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static io::BinaryReader* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "BinaryReader expected, got nothing");
                    return 0;
                }
                BinaryReaderWrapper* wrapper = luabridge::Stack<BinaryReaderWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "BinaryReader expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static io::BinaryReader* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                BinaryReaderWrapper* wrapper = luabridge::Stack<BinaryReaderWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            StreamWrapper get_stream() const
            {
                return StreamWrapper(This->getInputStream());
            }

            //---------------------------------------------------------
            int skip(lua_State* L)
            {
                int count = luaL_checkint(L, 2);
                if (count < 0) {
                    return luaL_error(L, "invalid count");
                }
                This->skipBytes(count);
                return 0;
            }

            //---------------------------------------------------------
            int readByte(lua_State* L)
            {
                u8 byte = This->readByte();
                lua_pushnumber(L, byte);
                return 1;
            }

            //---------------------------------------------------------
            int readBytes(lua_State* L)
            {
                int count = luaL_checkint(L, 2);
                if (count < 0) {
                    throw "invalid count";
                }
                ByteArray::Ptr bytearray = This->readByteArray(count);
                luabridge::push(L, ByteArrayWrapper(bytearray));
                return 1;
            }

            //---------------------------------------------------------
            int readString(lua_State* L)
            {
                int size = luaL_checkint(L, 2);
                if (size < 0) {
                    throw "invalid size";
                }
                std::string s = This->readString(size);
                lua_pushlstring(L, s.c_str(), s.size());
                return 1;
            }

            //---------------------------------------------------------
            int readBool(lua_State* L)
            {
                bool b = This->readBool();
                lua_pushboolean(L, b);
                return 1;
            }

            //---------------------------------------------------------
            int readInt8(lua_State* L)
            {
                i8 n = This->readInt8();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readInt16(lua_State* L)
            {
                i16 n = This->readInt16();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readInt16BE(lua_State* L)
            {
                i16 n = This->readInt16BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readInt32(lua_State* L)
            {
                i32 n = This->readInt32();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readInt32BE(lua_State* L)
            {
                i32 n = This->readInt32BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readUint8(lua_State* L)
            {
                u8 n = This->readUint8();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readUint16(lua_State* L)
            {
                u16 n = This->readUint16();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readUint16BE(lua_State* L)
            {
                u16 n = This->readUint16BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readUint32(lua_State* L)
            {
                u32 n = This->readUint32();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readUint32BE(lua_State* L)
            {
                u32 n = This->readUint32BE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readFloat(lua_State* L)
            {
                float n = This->readFloat();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readFloatBE(lua_State* L)
            {
                float n = This->readFloatBE();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readDouble(lua_State* L)
            {
                double n = This->readDouble();
                lua_pushnumber(L, n);
                return 1;
            }

            //---------------------------------------------------------
            int readDoubleBE(lua_State* L)
            {
                double n = This->readDoubleBE();
                lua_pushnumber(L, n);
                return 1;
            }
        };

        /**********************************************************
         *                   BINARY WRITER CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct BinaryWriterWrapper {
            io::BinaryWriter::Ptr This;

            //---------------------------------------------------------
            explicit BinaryWriterWrapper(io::BinaryWriter* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static io::BinaryWriter* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "BinaryWriter expected, got nothing");
                    return 0;
                }
                BinaryWriterWrapper* wrapper = luabridge::Stack<BinaryWriterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "BinaryWriter expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static io::BinaryWriter* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                BinaryWriterWrapper* wrapper = luabridge::Stack<BinaryWriterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            StreamWrapper get_stream() const
            {
                return StreamWrapper(This->getOutputStream());
            }

            //---------------------------------------------------------
            int writeByte(lua_State* L)
            {
                int byte = luaL_checkint(L, 2);
                int count = luaL_optint(L, 3, 1);
                if (count <= 0) {
                    return luaL_error(L, "invalid count");
                }
                This->writeByte(byte, count);
                return 0;
            }

            //---------------------------------------------------------
            int writeBytes(lua_State* L)
            {
                ByteArray* byte_array = ByteArrayWrapper::Get(L, 2);
                This->writeByteArray(byte_array);
                return 0;
            }

            //---------------------------------------------------------
            int writeString(lua_State* L)
            {
                const char* s = luaL_checkstring(L, 2);
                This->writeString(s);
                return 0;
            }

            //---------------------------------------------------------
            int writeBool(lua_State* L)
            {
                luaL_argcheck(L, lua_isnumber(L, 2) || lua_isboolean(L, 2), 2, "expected a number or boolean");
                bool b = lua_toboolean(L, 2);
                This->writeBool(b);
                return 0;
            }

            //---------------------------------------------------------
            int writeInt8(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt8(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeInt16(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt16(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeInt16BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt16BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeInt32(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt32(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeInt32BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeInt32BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeUint8(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint8(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeUint16(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint16(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeUint16BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint16BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeUint32(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint32(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeUint32BE(lua_State* L)
            {
                int n = luaL_checkint(L, 2);
                This->writeUint32BE(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeFloat(lua_State* L)
            {
                float n = luaL_checknumber(L, 2);
                This->writeFloat(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeFloatBE(lua_State* L)
            {
                float n = luaL_checknumber(L, 2);
                This->writeFloatBE(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeDouble(lua_State* L)
            {
                double n = luaL_checknumber(L, 2);
                This->writeDouble(n);
                return 0;
            }

            //---------------------------------------------------------
            int writeDoubleBE(lua_State* L)
            {
                double n = luaL_checknumber(L, 2);
                This->writeDoubleBE(n);
                return 0;
            }
        };

        /**********************************************************
         *                       SOUND CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct SoundWrapper {
            audio::Sound::Ptr This;

            //---------------------------------------------------------
            explicit SoundWrapper(audio::Sound* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static audio::Sound* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Sound expected, got nothing");
                    return 0;
                }
                SoundWrapper* wrapper = luabridge::Stack<SoundWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Sound expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static audio::Sound* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                SoundWrapper* wrapper = luabridge::Stack<SoundWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            bool get_isPlaying() const
            {
                return This->isPlaying();
            }

            //---------------------------------------------------------
            bool get_isSeekable() const
            {
                return This->isSeekable();
            }

            //---------------------------------------------------------
            int get_length() const
            {
                return This->getLength();
            }

            //---------------------------------------------------------
            bool get_repeat() const
            {
                return This->getRepeat();
            }

            //---------------------------------------------------------
            void set_repeat(bool repeat)
            {
                This->setRepeat(repeat);
            }

            //---------------------------------------------------------
            float get_volume() const
            {
                return This->getVolume();
            }

            //---------------------------------------------------------
            void set_volume(float volume)
            {
                if (volume < 0.0 || volume > 1.0) {
                    throw "invalid volume";
                }
                This->setVolume(volume);
            }

            //---------------------------------------------------------
            int get_position() const
            {
                return This->getPosition();
            }

            //---------------------------------------------------------
            void set_position(int position)
            {
                if (This->isSeekable()) {
                    if (position < 1 || position > This->getLength()) {
                        throw "invalid position";
                    }
                    This->setPosition(position - 1);
                }
            }

            //---------------------------------------------------------
            int __len(lua_State* L)
            {
                lua_pushnumber(L, This->getLength());
                return 1;
            }

            //---------------------------------------------------------
            int play(lua_State* L)
            {
                This->play();
                return 0;
            }

            //---------------------------------------------------------
            int stop(lua_State* L)
            {
                This->stop();
                return 0;
            }

            //---------------------------------------------------------
            int reset(lua_State* L)
            {
                This->reset();
                return 0;
            }
        };

        /**********************************************************
         *                    SOUND EFFECT CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct SoundEffectWrapper {
            audio::SoundEffect::Ptr This;

            //---------------------------------------------------------
            explicit SoundEffectWrapper(audio::SoundEffect* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static audio::SoundEffect* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "SoundEffect expected, got nothing");
                    return 0;
                }
                SoundEffectWrapper* wrapper = luabridge::Stack<SoundEffectWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "SoundEffect expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static audio::SoundEffect* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                SoundEffectWrapper* wrapper = luabridge::Stack<SoundEffectWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            float get_volume() const
            {
                return This->getVolume();
            }

            //---------------------------------------------------------
            void set_volume(float volume)
            {
                if (volume < 0.0 || volume > 1.0) {
                    throw "invalid volume";
                }
                This->setVolume(volume);
            }

            //---------------------------------------------------------
            int play(lua_State* L)
            {
                This->play();
                return 0;
            }

            //---------------------------------------------------------
            int stop(lua_State* L)
            {
                This->stop();
                return 0;
            }
        };

        /**********************************************************
         *                       IMAGE CLASS
         **********************************************************/

        //---------------------------------------------------------
        bool get_blend_mode_constant(int blend_mode, std::string& out_blend_mode_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (graphics::BlendMode::Set,      "set"     )
                (graphics::BlendMode::Mix,      "mix"     )
                (graphics::BlendMode::Add,      "add"     )
                (graphics::BlendMode::Subtract, "subtract")
                (graphics::BlendMode::Multiply, "multiply");

            map_type::iterator mapped_value = map.find(blend_mode);

            if (mapped_value == map.end()) {
                return false;
            }

            out_blend_mode_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_blend_mode_constant(const std::string& blend_mode_str, int& out_blend_mode)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("set",      graphics::BlendMode::Set     )
                ("mix",      graphics::BlendMode::Mix     )
                ("add",      graphics::BlendMode::Add     )
                ("subtract", graphics::BlendMode::Subtract)
                ("multiply", graphics::BlendMode::Multiply);

            map_type::iterator mapped_value = map.find(blend_mode_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_blend_mode = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        struct ImageWrapper {
            graphics::Image::Ptr This;

            //---------------------------------------------------------
            explicit ImageWrapper(graphics::Image* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static graphics::Image* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Image expected, got nothing");
                    return 0;
                }
                ImageWrapper* wrapper = luabridge::Stack<ImageWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Image expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static graphics::Image* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ImageWrapper* wrapper = luabridge::Stack<ImageWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            int get_width() const
            {
                return This->getWidth();
            }

            //---------------------------------------------------------
            int get_height() const
            {
                return This->getHeight();
            }

            //---------------------------------------------------------
            int __len(lua_State* L)
            {
                lua_pushnumber(L, This->getSizeInPixels());
                return 1;
            }

            //---------------------------------------------------------
            int getDimensions(lua_State* L)
            {
                lua_pushnumber(L, This->getWidth());
                lua_pushnumber(L, This->getHeight());
                return 2;
            }

            //---------------------------------------------------------
            int getBlendMode(lua_State* L)
            {
                std::string blend_mode_str;
                if (!get_blend_mode_constant(This->getBlendMode(), blend_mode_str)) {
                    throw "unexpected internal value";
                }
                lua_pushstring(L, blend_mode_str.c_str());
                return 1;
            }

            //---------------------------------------------------------
            int setBlendMode(lua_State* L)
            {
                const char* blend_mode_str = luaL_checkstring(L, 2);
                int blend_mode;
                if (!get_blend_mode_constant(blend_mode_str, blend_mode)) {
                    throw "invalid value";
                }
                This->setBlendMode(blend_mode);
                return 0;
            }

            //---------------------------------------------------------
            int getClipRect(lua_State* L)
            {
                Recti clip_rect = This->getClipRect();
                lua_pushnumber(L, clip_rect.getX());
                lua_pushnumber(L, clip_rect.getY());
                lua_pushnumber(L, clip_rect.getWidth());
                lua_pushnumber(L, clip_rect.getHeight());
                return 4;
            }

            //---------------------------------------------------------
            int setClipRect(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                int w = luaL_checkint(L, 4);
                int h = luaL_checkint(L, 5);

                Recti clip_rect = Recti(x, y, w, h);
                Recti image_bounds = Recti(This->getDimensions());

                if (!clip_rect.isValid() || !clip_rect.isInside(image_bounds)) {
                    throw "invalid rect";
                }

                This->setClipRect(clip_rect);
                return 0;
            }

            //---------------------------------------------------------
            int copyPixels(lua_State* L)
            {
                ByteArray::Ptr pixels = ByteArray::New((const u8*)This->getPixels(), This->getSizeInBytes());
                luabridge::push(L, ByteArrayWrapper(pixels));
                return 1;
            }

            //---------------------------------------------------------
            int copyRect(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                int w = luaL_checkint(L, 4);
                int h = luaL_checkint(L, 5);
                graphics::Image* destination = ImageWrapper::GetOpt(L, 6);

                Recti rect = Recti(x, y, w, h);
                Recti image_bounds = Recti(This->getDimensions());

                if (!rect.isValid() || !rect.isInside(image_bounds)) {
                    return luaL_error(L, "invalid rect");
                }

                luabridge::push(L, ImageWrapper(This->copyRect(rect, destination)));
                return 1;
            }

            //---------------------------------------------------------
            int resize(lua_State* L)
            {
                int width  = luaL_checkint(L, 2);
                int height = luaL_checkint(L, 3);
                luaL_argcheck(L, width  > 0, 2, "invalid width");
                luaL_argcheck(L, height > 0, 3, "invalid height");
                This->resize(width, height);
                return 0;
            }

            //---------------------------------------------------------
            int setAlpha(lua_State* L)
            {
                int alpha = luaL_checkint(L, 2);
                This->setAlpha(alpha);
                return 0;
            }

            //---------------------------------------------------------
            int clear(lua_State* L)
            {
                u32 color = luaL_optunsigned(L, 2, 0x000000FF);
                This->clear(graphics::RGBA8888ToRGBA(color));
                return 0;
            }

            //---------------------------------------------------------
            int grey(lua_State* L)
            {
                This->grey();
                return 0;
            }

            //---------------------------------------------------------
            int flip(lua_State* L)
            {
                const char* axis_str = luaL_checkstring(L, 2);
                if (std::strcmp(axis_str, "x-axis") == 0) {
                    This->flipVertical();
                } else if (std::strcmp(axis_str, "y-axis") == 0) {
                    This->flipHorizontal();
                } else if (std::strcmp(axis_str, "both axes") == 0) {
                    This->flipVertical();
                    This->flipHorizontal();
                } else {
                    throw "invalid axis constant";
                }
                return 0;
            }

            //---------------------------------------------------------
            int rotate(lua_State* L)
            {
                const char* direction_str = luaL_checkstring(L, 2);
                if (std::strcmp(direction_str, "clockwise") == 0) {
                    This->rotateClockwise();
                } else if (std::strcmp(direction_str, "counter-clockwise") == 0) {
                    This->rotateCounterClockwise();
                } else {
                    throw "invalid direction constant";
                }
                return 0;
            }

            //---------------------------------------------------------
            int getPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);

                luaL_argcheck(L, x >= 0 && x < This->getWidth(),  1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < This->getHeight(), 2, "invalid y");

                lua_pushunsigned(L, graphics::RGBAToRGBA8888(This->getPixel(x, y)));

                return 1;
            }

            //---------------------------------------------------------
            int setPixel(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                u32 c = luaL_checkunsigned(L, 4);

                luaL_argcheck(L, x >= 0 && x < This->getWidth(),  1, "invalid x");
                luaL_argcheck(L, y >= 0 && y < This->getHeight(), 2, "invalid y");

                This->setPixel(x, y, graphics::RGBA8888ToRGBA(c));

                return 0;
            }

            //---------------------------------------------------------
            int drawPoint(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                u32 c = luaL_checkunsigned(L, 4);

                This->drawPoint(
                    Vec2i(x, y),
                    graphics::RGBA8888ToRGBA(c)
                );

                return 0;
            }

            //---------------------------------------------------------
            int drawLine(lua_State* L)
            {
                int x1 = luaL_checkint(L, 2);
                int y1 = luaL_checkint(L, 3);
                int x2 = luaL_checkint(L, 4);
                int y2 = luaL_checkint(L, 5);
                u32 c1 = luaL_checkunsigned(L, 6);
                u32 c2 = luaL_optunsigned(L, 7, c1);

                This->drawLine(
                    Vec2i(x1, y1),
                    Vec2i(x2, y2),
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2)
                );

                return 0;
            }

            //---------------------------------------------------------
            int drawRectangle(lua_State* L)
            {
                bool fill = lua_toboolean(L, 2);
                int     x = luaL_checkint(L, 3);
                int     y = luaL_checkint(L, 4);
                int     w = luaL_checkint(L, 5);
                int     h = luaL_checkint(L, 6);
                u32    c1 = luaL_checkunsigned(L, 7);
                u32    c2 = luaL_optunsigned(L, 8, c1);
                u32    c3 = luaL_optunsigned(L, 9, c1);
                u32    c4 = luaL_optunsigned(L, 10, c1);

                This->drawRectangle(
                    fill,
                    Recti(x, y, w, h),
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2),
                    graphics::RGBA8888ToRGBA(c3),
                    graphics::RGBA8888ToRGBA(c4)
                );

                return 0;
            }

            //---------------------------------------------------------
            int drawCircle(lua_State* L)
            {
                bool fill = lua_toboolean(L, 2);
                int     x = luaL_checkint(L, 3);
                int     y = luaL_checkint(L, 4);
                int     r = luaL_checkint(L, 5);
                u32    c1 = luaL_checkunsigned(L, 6);
                u32    c2 = luaL_optunsigned(L, 7, c1);

                This->drawCircle(
                    fill,
                    Vec2i(x, y),
                    r,
                    graphics::RGBA8888ToRGBA(c1),
                    graphics::RGBA8888ToRGBA(c2)
                );

                return 0;
            }

            //---------------------------------------------------------
            int drawTriangle(lua_State* L)
            {
                /*
                bool fill = lua_toboolean(L, 2);
                int    x1 = luaL_checkint(L, 3);
                int    y1 = luaL_checkint(L, 4);
                int    x2 = luaL_checkint(L, 5);
                int    y2 = luaL_checkint(L, 6);
                int    x3 = luaL_checkint(L, 7);
                int    y3 = luaL_checkint(L, 8);
                u32    c1 = luaL_checkunsigned(L, 9);
                u32    c2 = luaL_optunsigned(L, 10, c1);
                u32    c3 = luaL_optunsigned(L, 11, c1);
                */

                // TODO

                return 0;
            }

            //---------------------------------------------------------
            int draw(lua_State* L)
            {
                graphics::Image* that = 0;
                int sx, sy, sw, sh;
                int x, y;
                float rotate;
                float scale;
                u32   color;

                int nargs = lua_gettop(L);
                if (nargs >= 8)
                {
                    that = ImageWrapper::Get(L, 2);
                    sx   = luaL_checkint(L, 3);
                    sy   = luaL_checkint(L, 4);
                    sw   = luaL_checkint(L, 5);
                    sh   = luaL_checkint(L, 6);
                    x    = luaL_checkint(L, 7);
                    y    = luaL_checkint(L, 8);
                    rotate = luaL_optnumber(L, 9, 0.0);
                    scale  = luaL_optnumber(L, 10, 1.0);
                    color  = luaL_optunsigned(L, 11, 0xFFFFFFFF);
                }
                else
                {
                    that = ImageWrapper::Get(L, 2);
                    x    = luaL_checkint(L, 3);
                    y    = luaL_checkint(L, 4);
                    rotate = luaL_optnumber(L, 5, 0.0);
                    scale  = luaL_optnumber(L, 6, 1.0);
                    color  = luaL_optunsigned(L, 7, 0xFFFFFFFF);

                    sx = 0;
                    sy = 0;
                    sw = that->getWidth();
                    sh = that->getHeight();
                }

                This->draw(
                    that,
                    Recti(sx, sy, sw, sh),
                    Vec2i(x, y),
                    rotate,
                    scale,
                    graphics::RGBA8888ToRGBA(color)
                );

                return 0;
            }

            //---------------------------------------------------------
            int drawq(lua_State* L)
            {
                graphics::Image* that = 0;
                int sx, sy, sw, sh;
                int x1, y1;
                int x2, y2;
                int x3, y3;
                int x4, y4;
                u32 color;

                int nargs = lua_gettop(L);
                if (nargs >= 14)
                {
                    that = ImageWrapper::Get(L, 2);
                    sx   = luaL_checkint(L, 3);
                    sy   = luaL_checkint(L, 4);
                    sw   = luaL_checkint(L, 5);
                    sh   = luaL_checkint(L, 6);
                    x1   = luaL_checkint(L, 7);
                    y1   = luaL_checkint(L, 8);
                    x2   = luaL_checkint(L, 9);
                    y2   = luaL_checkint(L, 10);
                    x3   = luaL_checkint(L, 11);
                    y3   = luaL_checkint(L, 12);
                    x4   = luaL_checkint(L, 13);
                    y4   = luaL_checkint(L, 14);
                    color = luaL_optunsigned(L, 15, 0xFFFFFFFF);
                }
                else
                {
                    that = ImageWrapper::Get(L, 2);
                    x1   = luaL_checkint(L, 3);
                    y1   = luaL_checkint(L, 4);
                    x2   = luaL_checkint(L, 5);
                    y2   = luaL_checkint(L, 6);
                    x3   = luaL_checkint(L, 7);
                    y3   = luaL_checkint(L, 8);
                    x4   = luaL_checkint(L, 9);
                    y4   = luaL_checkint(L, 10);
                    color = luaL_optunsigned(L, 11, 0xFFFFFFFF);

                    sx = 0;
                    sy = 0;
                    sw = that->getWidth();
                    sh = that->getHeight();
                }

                This->drawq(
                    that,
                    Recti(sx, sy, sw, sh),
                    Vec2i(x1, y1),
                    Vec2i(x2, y2),
                    Vec2i(x3, y3),
                    Vec2i(x4, y4),
                    graphics::RGBA8888ToRGBA(color)
                );

                return 0;
            }
        };

        /**********************************************************
         *                     CHARACTER CLASS
         **********************************************************/

        //---------------------------------------------------------
        bool get_direction_constant(int direction, std::string& out_direction_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::DIR_UP,         "up"        )
                (RPG::DIR_RIGHT,      "right"     )
                (RPG::DIR_DOWN,       "down"      )
                (RPG::DIR_LEFT,       "left"      )
                (RPG::DIR_UP_RIGHT,   "up right"  )
                (RPG::DIR_DOWN_RIGHT, "down right")
                (RPG::DIR_DOWN_LEFT,  "down left" )
                (RPG::DIR_UP_LEFT,    "up left"   );

            map_type::iterator mapped_value = map.find(direction);

            if (mapped_value == map.end()) {
                return false;
            }

            out_direction_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_direction_constant(const std::string& direction_str, int& out_direction)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("up",         RPG::DIR_UP        )
                ("right",      RPG::DIR_RIGHT     )
                ("down",       RPG::DIR_DOWN      )
                ("left",       RPG::DIR_LEFT      )
                ("up right",   RPG::DIR_UP_RIGHT  )
                ("down right", RPG::DIR_DOWN_RIGHT)
                ("down left",  RPG::DIR_DOWN_LEFT )
                ("up left",    RPG::DIR_UP_LEFT   );

            map_type::iterator mapped_value = map.find(direction_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_direction = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_facing_constant(int facing, std::string& out_facing_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::FACE_UP,    "up"   )
                (RPG::FACE_RIGHT, "right")
                (RPG::FACE_DOWN,  "down" )
                (RPG::FACE_LEFT,  "left" );

            map_type::iterator mapped_value = map.find(facing);

            if (mapped_value == map.end()) {
                return false;
            }

            out_facing_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_facing_constant(const std::string& facing_str, int& out_facing)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("up",    RPG::FACE_UP   )
                ("right", RPG::FACE_RIGHT)
                ("down",  RPG::FACE_DOWN )
                ("left",  RPG::FACE_LEFT );

            map_type::iterator mapped_value = map.find(facing_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_facing = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_layer_constant(int layer, std::string& out_layer_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::LAYER_BELOW_HERO,         "below hero"        )
                (RPG::LAYER_SAME_LEVEL_AS_HERO, "same level as hero")
                (RPG::LAYER_ABOVE_HERO,         "above hero"        );

            map_type::iterator mapped_value = map.find(layer);

            if (mapped_value == map.end()) {
                return false;
            }

            out_layer_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_layer_constant(const std::string& layer_str, int& out_layer)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("below hero",         RPG::LAYER_BELOW_HERO        )
                ("same level as hero", RPG::LAYER_SAME_LEVEL_AS_HERO)
                ("above hero",         RPG::LAYER_ABOVE_HERO        );

            map_type::iterator mapped_value = map.find(layer_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_layer = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_animationtype_constant(int animationtype, std::string& out_animationtype_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::ANI_NORMAL,             "normal"            )
                (RPG::ANI_STEPPING,           "stepping"          )
                (RPG::ANI_FIXED_DIR_NORMAL,   "fixed dir normal"  )
                (RPG::ANI_FIXED_DIR_STEPPING, "fixed dir stepping")
                (RPG::ANI_FIXED_GRAPHIC,      "fixed graphic"     )
                (RPG::ANI_SPIN_AROUND,        "spin around"       );

            map_type::iterator mapped_value = map.find(animationtype);

            if (mapped_value == map.end()) {
                return false;
            }

            out_animationtype_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_animationtype_constant(const std::string& animationtype_str, int& out_animationtype)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("normal",             RPG::ANI_NORMAL            )
                ("stepping",           RPG::ANI_STEPPING          )
                ("fixed dir normal",   RPG::ANI_FIXED_DIR_NORMAL  )
                ("fixed dir stepping", RPG::ANI_FIXED_DIR_STEPPING)
                ("fixed graphic",      RPG::ANI_FIXED_GRAPHIC     )
                ("spin around",        RPG::ANI_SPIN_AROUND       );

            map_type::iterator mapped_value = map.find(animationtype_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_animationtype = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        struct CharacterWrapper {
            RPG::Character* This;

            //---------------------------------------------------------
            explicit CharacterWrapper(RPG::Character* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static RPG::Character* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Character expected, got nothing");
                    return 0;
                }
                CharacterWrapper* wrapper = luabridge::Stack<CharacterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Character expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static RPG::Character* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                CharacterWrapper* wrapper = luabridge::Stack<CharacterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            std::string get_name() const
            {
                return const_cast<RPG::Character*>(This)->getName();
            }

            //---------------------------------------------------------
            int get_id() const
            {
                return const_cast<RPG::Character*>(This)->id;
            }

            //---------------------------------------------------------
            std::string get_charsetFilename() const
            {
                return const_cast<RPG::Character*>(This)->charsetFilename.s_str();
            }

            //---------------------------------------------------------
            int get_charsetID() const
            {
                return const_cast<RPG::Character*>(This)->charsetId;
            }

            //---------------------------------------------------------
            bool get_enabled() const
            {
                return const_cast<RPG::Character*>(This)->enabled;
            }

            //---------------------------------------------------------
            void set_enabled(bool enabled)
            {
                This->enabled = enabled;
            }

            //---------------------------------------------------------
            int get_screenX() const
            {
                return const_cast<RPG::Character*>(This)->getScreenX();
            }

            //---------------------------------------------------------
            int get_screenY() const
            {
                return const_cast<RPG::Character*>(This)->getScreenY();
            }

            //---------------------------------------------------------
            int get_x() const
            {
                return const_cast<RPG::Character*>(This)->x;
            }

            //---------------------------------------------------------
            void set_x(int x)
            {
                This->x = x;
            }

            //---------------------------------------------------------
            int get_y() const
            {
                return const_cast<RPG::Character*>(This)->y;
            }

            //---------------------------------------------------------
            void set_y(int y)
            {
                This->y = y;
            }

            //---------------------------------------------------------
            int get_offsetX() const
            {
                return const_cast<RPG::Character*>(This)->offsetX;
            }

            //---------------------------------------------------------
            void set_offsetX(int offsetX)
            {
                This->offsetX = offsetX;
            }

            //---------------------------------------------------------
            int get_offsetY() const
            {
                return const_cast<RPG::Character*>(This)->offsetY;
            }

            //---------------------------------------------------------
            void set_offsetY(int offsetY)
            {
                This->offsetY = offsetY;
            }

            //---------------------------------------------------------
            std::string get_direction() const
            {
                std::string direction_str;
                if (!get_direction_constant(This->direction, direction_str)) {
                    throw "unexpected internal value";
                }
                return direction_str;
            }

            //---------------------------------------------------------
            void set_direction(const std::string& direction_str)
            {
                int direction;
                if (!get_direction_constant(direction_str, direction)) {
                    throw "invalid value";
                }
                This->direction = direction;
            }

            //---------------------------------------------------------
            std::string get_facing() const
            {
                std::string facing_str;
                if (!get_facing_constant(This->facing, facing_str)) {
                    throw "unexpected internal value";
                }
                return facing_str;
            }

            //---------------------------------------------------------
            void set_facing(const std::string& facing_str)
            {
                int facing;
                if (!get_facing_constant(facing_str, facing)) {
                    throw "invalid value";
                }
                This->facing = facing;
            }

            //---------------------------------------------------------
            int get_step() const
            {
                return const_cast<RPG::Character*>(This)->step;
            }

            //---------------------------------------------------------
            void set_step(int step)
            {
                if (step < 0 || step > 3) {
                    throw "invalid value";
                }
                This->step = step;
            }

            //---------------------------------------------------------
            int get_transparency() const
            {
                return const_cast<RPG::Character*>(This)->transparency;
            }

            //---------------------------------------------------------
            void set_transparency(int transparency)
            {
                if (transparency < 0 || transparency > 8) {
                    throw "invalid value";
                }
                This->transparency = transparency;
            }

            //---------------------------------------------------------
            int get_frequency() const
            {
                return const_cast<RPG::Character*>(This)->frequency;
            }

            //---------------------------------------------------------
            void set_frequency(int frequency)
            {
                if (frequency < 1 || frequency > 8) {
                    throw "invalid value";
                }
                This->frequency = frequency;
            }

            //---------------------------------------------------------
            std::string get_layer() const
            {
                std::string layer_str;
                if (!get_layer_constant(This->layer, layer_str)) {
                    throw "unexpected internal value";
                }
                return layer_str;
            }

            //---------------------------------------------------------
            void set_layer(const std::string& layer_str)
            {
                int layer;
                if (!get_layer_constant(layer_str, layer)) {
                    throw "invalid value";
                }
                This->layer = layer;
            }

            //---------------------------------------------------------
            bool get_forbidEventOverlap() const
            {
                return const_cast<RPG::Character*>(This)->forbidEventOverlap;
            }

            //---------------------------------------------------------
            void set_forbidEventOverlap(bool forbidEventOverlap)
            {
                This->forbidEventOverlap = forbidEventOverlap;
            }

            //---------------------------------------------------------
            std::string get_animationType() const
            {
                std::string animationType_str;
                if (!get_animationtype_constant(This->animationType, animationType_str)) {
                    throw "unexpected internal value";
                }
                return animationType_str;
            }

            //---------------------------------------------------------
            void set_animationType(const std::string& animationType_str)
            {
                int animationType;
                if (!get_animationtype_constant(animationType_str, animationType)) {
                    throw "invalid value";
                }
                This->animationType = animationType;
            }

            //---------------------------------------------------------
            bool get_fixedDirection() const
            {
                return const_cast<RPG::Character*>(This)->fixedDirection;
            }

            //---------------------------------------------------------
            void set_fixedDirection(bool fixedDirection)
            {
                This->fixedDirection = fixedDirection;
            }

            //---------------------------------------------------------
            int get_speed() const
            {
                return const_cast<RPG::Character*>(This)->speed;
            }

            //---------------------------------------------------------
            void set_speed(int speed)
            {
                if (speed < 1 || speed > 6) {
                    throw "invalid value";
                }
                This->speed = speed;
            }

            //---------------------------------------------------------
            bool get_moving() const
            {
                return const_cast<RPG::Character*>(This)->moving;
            }

            //---------------------------------------------------------
            void set_moving(bool moving)
            {
                This->moving = moving;
            }

            //---------------------------------------------------------
            bool get_phasing() const
            {
                return const_cast<RPG::Character*>(This)->phasing;
            }

            //---------------------------------------------------------
            void set_phasing(bool phasing)
            {
                This->phasing = phasing;
            }

            int getScreenPosition(lua_State* L)
            {
                lua_pushnumber(L, This->getScreenX());
                lua_pushnumber(L, This->getScreenY());
                return 2;
            }

            int getPosition(lua_State* L)
            {
                lua_pushnumber(L, This->x);
                lua_pushnumber(L, This->y);
                return 2;
            }

            int setPosition(lua_State* L)
            {
                int x = luaL_checkint(L, 2);
                int y = luaL_checkint(L, 3);
                This->x = x;
                This->y = y;
                return 0;
            }

            int getPositionOffset(lua_State* L)
            {
                lua_pushnumber(L, This->offsetX);
                lua_pushnumber(L, This->offsetY);
                return 2;
            }

            int setPositionOffset(lua_State* L)
            {
                int offsetX = luaL_checkint(L, 2);
                int offsetY = luaL_checkint(L, 3);
                This->offsetX = offsetX;
                This->offsetY = offsetY;
                return 0;
            }

            //---------------------------------------------------------
            int setFixedStep(lua_State* L)
            {
                int newStep = luaL_checkint(L, 2);
                if (newStep < 0 || newStep > 3) {
                    throw "invalid step value";
                }
                This->setFixedStep(newStep);
                return 0;
            }

            //---------------------------------------------------------
            int setNormalStep(lua_State* L)
            {
                This->setNormalStep();
                return 0;
            }

            //---------------------------------------------------------
            int isMovePossible(lua_State* L)
            {
                int fromX = luaL_checkint(L, 2);
                int fromY = luaL_checkint(L, 3);
                int toX   = luaL_checkint(L, 4);
                int toY   = luaL_checkint(L, 5);
                lua_pushboolean(L, This->isMovePossible(fromX, fromY, toX, toY));
                return 1;
            }

            //---------------------------------------------------------
            int stop(lua_State* L)
            {
                This->stop();
                return 0;
            }

            //---------------------------------------------------------
            int doStep(lua_State* L)
            {
                const char* direction_str = luaL_checkstring(L, 2);
                int direction;
                if (!get_direction_constant(direction_str, direction)) {
                    throw "invalid direction constant";
                }
                This->doStep((RPG::Direction)direction);
                return 0;
            }
        };

        /**********************************************************
         *                       EVENT CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct EventWrapper : public CharacterWrapper {
            RPG::Event* This;

            //---------------------------------------------------------
            explicit EventWrapper(RPG::Event* ptr)
                : CharacterWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            static RPG::Event* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Event expected, got nothing");
                    return 0;
                }
                EventWrapper* wrapper = luabridge::Stack<EventWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Event expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static RPG::Event* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                EventWrapper* wrapper = luabridge::Stack<EventWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

        };

        /**********************************************************
         *                        HERO CLASS
         **********************************************************/

        //---------------------------------------------------------
        bool get_controlmode_constant(int controlmode, std::string& out_controlmode_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::CONTROL_EVERYTHING,                   "control everything"                )
                (RPG::CONTROL_EVERYTHING_EXCEPT_MOVEMENT,   "control everything except movement")
                (RPG::CONTROL_NOTHING,                      "control nothing"                   );

            map_type::iterator mapped_value = map.find(controlmode);

            if (mapped_value == map.end()) {
                return false;
            }

            out_controlmode_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_controlmode_constant(const std::string& controlmode_str, int& out_controlmode)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("control everything",                  RPG::CONTROL_EVERYTHING                )
                ("control everything except movement",  RPG::CONTROL_EVERYTHING_EXCEPT_MOVEMENT)
                ("control nothing",                     RPG::CONTROL_NOTHING                   );

            map_type::iterator mapped_value = map.find(controlmode_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_controlmode = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_vehicle_constant(int vehicle, std::string& out_vehicle_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::HV_NONE,      "none"   )
                (RPG::HV_SKIFF,     "skiff"  )
                (RPG::HV_SHIP,      "ship"   )
                (RPG::HV_AIRSHIP,   "airship");

            map_type::iterator mapped_value = map.find(vehicle);

            if (mapped_value == map.end()) {
                return false;
            }

            out_vehicle_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_vehicle_constant(const std::string& vehicle_str, int& out_vehicle)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("none",    RPG::HV_NONE   )
                ("skiff",   RPG::HV_SKIFF  )
                ("ship",    RPG::HV_SHIP   )
                ("airship", RPG::HV_AIRSHIP);

            map_type::iterator mapped_value = map.find(vehicle_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_vehicle = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        struct HeroWrapper : public CharacterWrapper {
            RPG::Hero* This;

            //---------------------------------------------------------
            explicit HeroWrapper(RPG::Hero* ptr)
                : CharacterWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            static RPG::Hero* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Hero expected, got nothing");
                    return 0;
                }
                HeroWrapper* wrapper = luabridge::Stack<HeroWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Hero expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static RPG::Hero* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                HeroWrapper* wrapper = luabridge::Stack<HeroWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            std::string get_controlMode() const
            {
                std::string controlmode_str;
                if (!get_controlmode_constant(const_cast<RPG::Hero*>(This)->getControlMode(), controlmode_str)) {
                    throw "unexpected internal value";
                }
                return controlmode_str;
            }

            //---------------------------------------------------------
            void set_controlMode(const std::string& controlmode_str)
            {
                int controlmode;
                if (!get_controlmode_constant(controlmode_str, controlmode)) {
                    throw "invalid value";
                }
                This->setControlMode((RPG::HeroControlMode)controlmode);
            }

            //---------------------------------------------------------
            std::string get_vehicle() const
            {
                std::string vehicle_str;
                if (!get_vehicle_constant(const_cast<RPG::Hero*>(This)->vehicle, vehicle_str)) {
                    throw "unexpected internal value";
                }
                return vehicle_str;
            }
        };

        /**********************************************************
         *                     BATTLER CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct BattlerWrapper {
            RPG::Battler* This;

            //---------------------------------------------------------
            explicit BattlerWrapper(RPG::Battler* ptr)
                : This(ptr)
            {
            }

            //---------------------------------------------------------
            static RPG::Battler* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Battler expected, got nothing");
                    return 0;
                }
                BattlerWrapper* wrapper = luabridge::Stack<BattlerWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Battler expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static RPG::Battler* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                BattlerWrapper* wrapper = luabridge::Stack<BattlerWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            int get_databaseID() const
            {
                if (const_cast<RPG::Battler*>(This)->isMonster()) {
                    return static_cast<const RPG::Monster*>(This)->databaseId;
                } else {
                    return const_cast<RPG::Battler*>(This)->id;
                }
            }

            //---------------------------------------------------------
            std::string get_name() const
            {
                return const_cast<RPG::Battler*>(This)->getName();
            }

            //---------------------------------------------------------
            bool get_isMonster() const
            {
                return const_cast<RPG::Battler*>(This)->isMonster();
            }

            //---------------------------------------------------------
            bool get_notHidden() const
            {
                return const_cast<RPG::Battler*>(This)->notHidden;
            }

            //---------------------------------------------------------
            bool get_mirrored() const
            {
                return const_cast<RPG::Battler*>(This)->mirrored;
            }

            //---------------------------------------------------------
            int get_hp() const
            {
                return const_cast<RPG::Battler*>(This)->hp;
            }

            //---------------------------------------------------------
            void set_hp(int hp)
            {
                if (hp < 0 || hp > This->getMaxHp()) {
                    throw "invalid hp";
                }
                This->hp = hp;
            }

            //---------------------------------------------------------
            int get_mp() const
            {
                return const_cast<RPG::Battler*>(This)->mp;
            }

            //---------------------------------------------------------
            void set_mp(int mp)
            {
                if (mp < 0 || mp > This->getMaxMp()) {
                    throw "invalid mp";
                }
                This->mp = mp;
            }

            //---------------------------------------------------------
            int get_maxHP() const
            {
                return const_cast<RPG::Battler*>(This)->getMaxHp();
            }

            //---------------------------------------------------------
            int get_maxMP() const
            {
                return const_cast<RPG::Battler*>(This)->getMaxMp();
            }

            //---------------------------------------------------------
            int get_attack() const
            {
                return const_cast<RPG::Battler*>(This)->getAttack();
            }

            //---------------------------------------------------------
            int get_defense() const
            {
                return const_cast<RPG::Battler*>(This)->getDefense();
            }

            //---------------------------------------------------------
            int get_intelligence() const
            {
                return const_cast<RPG::Battler*>(This)->getIntelligence();
            }

            //---------------------------------------------------------
            int get_agility() const
            {
                return const_cast<RPG::Battler*>(This)->getAgility();
            }

            //---------------------------------------------------------
            int get_atbValue() const
            {
                return const_cast<RPG::Battler*>(This)->atbValue;
            }

            //---------------------------------------------------------
            int get_turnsTaken() const
            {
                return const_cast<RPG::Battler*>(This)->turnsTaken;
            }

            //---------------------------------------------------------
            int get_screenX() const
            {
                return This->x;
            }

            //---------------------------------------------------------
            int get_screenY() const
            {
                return This->y;
            }

            int getScreenPosition(lua_State* L)
            {
                lua_pushnumber(L, This->x);
                lua_pushnumber(L, This->y);
                return 2;
            }
        };

        /**********************************************************
         *                      ACTOR CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct ActorWrapper : public BattlerWrapper {
            RPG::Actor* This;

            //---------------------------------------------------------
            explicit ActorWrapper(RPG::Actor* ptr)
                : BattlerWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            static RPG::Actor* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Actor expected, got nothing");
                    return 0;
                }
                ActorWrapper* wrapper = luabridge::Stack<ActorWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Actor expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static RPG::Actor* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                ActorWrapper* wrapper = luabridge::Stack<ActorWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            std::string get_degree() const
            {
                return const_cast<RPG::Actor*>(This)->getDegree();
            }

            //---------------------------------------------------------
            std::string get_charsetFilename() const
            {
                return const_cast<RPG::Actor*>(This)->getCharsetFilename();
            }

            //---------------------------------------------------------
            int get_charsetID() const
            {
                return const_cast<RPG::Actor*>(This)->getCharsetId();
            }

            //---------------------------------------------------------
            std::string get_facesetFilename() const
            {
                return const_cast<RPG::Actor*>(This)->getFacesetFilename();
            }

            //---------------------------------------------------------
            int get_facesetID() const
            {
                return const_cast<RPG::Actor*>(This)->getFacesetId();
            }

            //---------------------------------------------------------
            int get_level() const
            {
                return const_cast<RPG::Actor*>(This)->level;
            }

            //---------------------------------------------------------
            void set_level(int level)
            {
                if (level < 1 || level > 99) {
                    throw "invalid value";
                }
                This->level = level;
            }

            //---------------------------------------------------------
            int get_exp() const
            {
                return const_cast<RPG::Actor*>(This)->exp;
            }

            //---------------------------------------------------------
            void set_exp(int exp)
            {
                if (exp < 0) {
                    throw "invalid value";
                }
                This->exp = exp;
            }

            //---------------------------------------------------------
            int get_weaponID() const
            {
                return const_cast<RPG::Actor*>(This)->weaponId;
            }

            //---------------------------------------------------------
            void set_weaponID(int weaponId)
            {
                if (weaponId < 0) {
                    throw "invalid value";
                }
                This->weaponId = weaponId;
            }

            //---------------------------------------------------------
            int get_shieldID() const
            {
                return const_cast<RPG::Actor*>(This)->shieldId;
            }

            //---------------------------------------------------------
            void set_shieldID(int shieldId)
            {
                if (shieldId < 0) {
                    throw "invalid value";
                }
                This->shieldId = shieldId;
            }

            //---------------------------------------------------------
            int get_armorID() const
            {
                return const_cast<RPG::Actor*>(This)->armorId;
            }

            //---------------------------------------------------------
            void set_armorID(int armorId)
            {
                if (armorId < 0) {
                    throw "invalid value";
                }
                This->armorId = armorId;
            }

            //---------------------------------------------------------
            int get_helmetID() const
            {
                return const_cast<RPG::Actor*>(This)->helmetId;
            }

            //---------------------------------------------------------
            void set_helmetID(int helmetId)
            {
                if (helmetId < 0) {
                    throw "invalid value";
                }
                This->helmetId = helmetId;
            }

            //---------------------------------------------------------
            int get_accessoryID() const
            {
                return const_cast<RPG::Actor*>(This)->accessoryId;
            }

            //---------------------------------------------------------
            void set_accessoryID(int accessoryId)
            {
                if (accessoryId < 0) {
                    throw "invalid value";
                }
                This->accessoryId = accessoryId;
            }

            //---------------------------------------------------------
            bool get_twoWeapons() const
            {
                return const_cast<RPG::Actor*>(This)->twoWeapons;
            }

            //---------------------------------------------------------
            void set_twoWeapons(bool twoWeapons)
            {
                This->twoWeapons = twoWeapons;
            }

            //---------------------------------------------------------
            bool get_lockEquipment() const
            {
                return const_cast<RPG::Actor*>(This)->lockEquipemnt;
            }

            //---------------------------------------------------------
            void set_lockEquipment(bool lockEquipment)
            {
                This->lockEquipemnt = lockEquipment;
            }

            //---------------------------------------------------------
            bool get_autoBattle() const
            {
                return const_cast<RPG::Actor*>(This)->autoBattle;
            }

            //---------------------------------------------------------
            void set_autoBattle(bool autoBattle)
            {
                This->autoBattle = autoBattle;
            }

            //---------------------------------------------------------
            bool get_mightyGuard() const
            {
                return const_cast<RPG::Actor*>(This)->mightyGuard;
            }

            //---------------------------------------------------------
            void set_mightyGuard(bool mightyGuard)
            {
                This->mightyGuard = mightyGuard;
            }
        };

        /**********************************************************
         *                     MONSTER CLASS
         **********************************************************/

        //---------------------------------------------------------
        struct MonsterWrapper : public BattlerWrapper {
            RPG::Monster* This;

            //---------------------------------------------------------
            explicit MonsterWrapper(RPG::Monster* ptr)
                : BattlerWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            static RPG::Monster* Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Monster expected, got nothing");
                    return 0;
                }
                MonsterWrapper* wrapper = luabridge::Stack<MonsterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Monster expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            static RPG::Monster* GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                MonsterWrapper* wrapper = luabridge::Stack<MonsterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            int get_monsterPartyID() const
            {
                return This->id + 1;
            }
        };

        /**********************************************************
         *                       MAP MODULE
         **********************************************************/

        //---------------------------------------------------------
        HeroWrapper map_get_hero()
        {
            return HeroWrapper(RPG::hero);
        }

        //---------------------------------------------------------
        int map_events_proxy_table_indexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            if (n < 1 || n > RPG::map->events.count()) {
               return luaL_error(L, "index out of bounds");
            }
            luabridge::push(L, EventWrapper(RPG::map->events.ptr->list[n-1]));
            return 1;
        }

        //---------------------------------------------------------
        int map_events_proxy_table_newindexMetaMethod(lua_State* L)
        {
           return luaL_error(L, "operation not allowed");
        }

        //---------------------------------------------------------
        int map_events_proxy_table_lenMetaMethod(lua_State* L)
        {
            lua_pushnumber(L, RPG::map->events.count());
            return 1;
        }

        //---------------------------------------------------------
        luabridge::LuaRef map_get_events()
        {
            lua_State* L = Context::Current().interpreter();

            lua_pushstring(L, "__rpg_map_events_proxy_table");
            lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

            if (!lua_istable(L, -1)) {
                lua_newtable(L); // proxy table

                lua_newtable(L); // metatable

                lua_pushstring(L, "__index");
                lua_pushcfunction(L, map_events_proxy_table_indexMetaMethod);
                lua_rawset(L, -3); // add __index() to metatable

                lua_pushstring(L, "__newindex");
                lua_pushcfunction(L, map_events_proxy_table_newindexMetaMethod);
                lua_rawset(L, -3); // add __newindex() to metatable

                lua_pushstring(L, "__len");
                lua_pushcfunction(L, map_events_proxy_table_lenMetaMethod);
                lua_rawset(L, -3); // add __len() to metatable

                lua_setmetatable(L, -2); // set metatable for proxy table

                lua_pushstring(L, "__rpg_map_events_proxy_table");
                lua_pushvalue(L, -2); // push proxy table
                lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                assert(lua_istable(L, -1));
            }

            return luabridge::LuaRef::fromStack(L, -1);
        }

        //---------------------------------------------------------
        int map_get_id()
        {
            return RPG::hero->mapId;
        }

        //---------------------------------------------------------
        int map_get_width()
        {
            return RPG::map->getWidth();
        }

        //---------------------------------------------------------
        int map_get_height()
        {
            return RPG::map->getHeight();
        }

        //---------------------------------------------------------
        int map_get_cameraX()
        {
            return RPG::map->getCameraX();
        }

        //---------------------------------------------------------
        int map_get_cameraY()
        {
            return RPG::map->getCameraY();
        }

        //---------------------------------------------------------
        int map_findEventByID(lua_State* L)
        {
            int id = luaL_checkint(L, 1);
            RPG::Event* event = RPG::map->events.get(id);
            if (event) {
                luabridge::push(L, EventWrapper(event));
            } else {
                lua_pushnil(L);
            }
            return 1;
        }

        //---------------------------------------------------------
        int map_findEventByName(lua_State* L)
        {
            const char* name = luaL_checkstring(L, 1);
            for (int i = 0; i < RPG::map->events.count(); ++i) {
                RPG::Event* event = RPG::map->events.ptr->list[i];
                if (std::strcmp(event->getName().c_str(), name) == 0) {
                    luabridge::push(L, EventWrapper(event));
                    return 1;
                }
            }
            lua_pushnil(L);
            return 1;
        }

        //---------------------------------------------------------
        int map_getCameraPosition(lua_State* L)
        {
            lua_pushnumber(L, RPG::map->getCameraX());
            lua_pushnumber(L, RPG::map->getCameraY());
            return 2;
        }

        //---------------------------------------------------------
        int map_setCameraPosition(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);
            RPG::map->setCamera(x, y);
            return 0;
        }

        //---------------------------------------------------------
        int map_moveCamera(lua_State* L)
        {
            int ox = luaL_checkint(L, 1);
            int oy = luaL_checkint(L, 2);
            int speed = luaL_checkint(L, 3);
            RPG::map->moveCamera(ox, oy, speed);
            return 0;
        }

        //---------------------------------------------------------
        int map_getLowerLayerTileID(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);
            if (x * y < 0 || x >= RPG::map->getWidth() || y >= RPG::map->getHeight()) {
                throw "invalid x or y";
            }
            lua_pushnumber(L, RPG::getLowerLayerTileId(RPG::map, x, y));
            return 1;
        }

        //---------------------------------------------------------
        int map_getUpperLayerTileID(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);
            if (x * y < 0 || x >= RPG::map->getWidth() || y >= RPG::map->getHeight()) {
                throw "invalid x or y";
            }
            lua_pushnumber(L, RPG::getUpperLayerTileId(RPG::map, x, y));
            return 1;
        }

        //---------------------------------------------------------
        int map_getTerrainID(lua_State* L)
        {
            int tileID = luaL_checkint(L, 1);
            if (tileID < 0) {
                return luaL_error(L, "invalid tile id");
            }
            lua_pushnumber(L, RPG::getTerrainId(RPG::map, tileID));
            return 1;
        }

        /**********************************************************
         *                      BATTLE MODULE
         **********************************************************/

        //---------------------------------------------------------
        int battle_monster_party_proxy_table_indexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            if (n < 1 || n > RPG::monsters.count()) {
               return luaL_error(L, "index out of bounds");
            }
            luabridge::push(L, MonsterWrapper(RPG::monsters[n-1]));
            return 1;
        }

        //---------------------------------------------------------
        int battle_monster_party_proxy_table_newindexMetaMethod(lua_State* L)
        {
           return luaL_error(L, "operation not allowed");
        }

        //---------------------------------------------------------
        int battle_monster_party_proxy_table_lenMetaMethod(lua_State* L)
        {
            lua_pushnumber(L, RPG::monsters.count());
            return 1;
        }

        //---------------------------------------------------------
        luabridge::LuaRef battle_get_monsterParty()
        {
            if (RPG::system->scene != RPG::SCENE_BATTLE) {
                throw "trying to access monster party while not in battle";
            }

            lua_State* L = Context::Current().interpreter();

            lua_pushstring(L, "__rpg_battle_monster_party_proxy_table");
            lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

            if (!lua_istable(L, -1)) {
                lua_newtable(L); // proxy table

                lua_newtable(L); // metatable

                lua_pushstring(L, "__index");
                lua_pushcfunction(L, battle_monster_party_proxy_table_indexMetaMethod);
                lua_rawset(L, -3); // add __index() to metatable

                lua_pushstring(L, "__newindex");
                lua_pushcfunction(L, battle_monster_party_proxy_table_newindexMetaMethod);
                lua_rawset(L, -3); // add __newindex() to metatable

                lua_pushstring(L, "__len");
                lua_pushcfunction(L, battle_monster_party_proxy_table_lenMetaMethod);
                lua_rawset(L, -3); // add __len() to metatable

                lua_setmetatable(L, -2); // set metatable for proxy table

                lua_pushstring(L, "__rpg_battle_monster_party_proxy_table");
                lua_pushvalue(L, -2); // push proxy table
                lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                assert(lua_istable(L, -1));
            }

            return luabridge::LuaRef::fromStack(L, -1);
        }

        /***********************************************************
         *                        I/O MODULE
         **********************************************************/

        //---------------------------------------------------------
        int io_newMemoryStream(lua_State* L)
        {
            int nargs = lua_gettop(L);
            if (nargs == 0)
            {
                // newMemoryStream()
                io::MemoryStream::Ptr memory_stream = io::MemoryStream::New();
                luabridge::push(L, MemoryStreamWrapper(memory_stream));
            }
            else if (nargs >= 1 && lua_isnumber(L, 1))
            {
                // newMemoryStream(capacity [, value])
                int capacity = luaL_checkint(L, 1);
                int value = luaL_optint(L, 2, 0);
                io::MemoryStream::Ptr memory_stream = io::MemoryStream::New(capacity, value);
                luabridge::push(L, MemoryStreamWrapper(memory_stream));
            }
            else
            {
                // newMemoryStream(bytearray)
                ByteArray* byte_array = ByteArrayWrapper::Get(L, 1);
                io::MemoryStream::Ptr memory_stream = io::MemoryStream::New(byte_array);
                luabridge::push(L, MemoryStreamWrapper(memory_stream));
            }
            return 1;
        }

        //---------------------------------------------------------
        int io_newBinaryReader(lua_State* L)
        {
            io::Stream* stream = StreamWrapper::Get(L, 1);
            io::BinaryReader::Ptr binary_reader = io::BinaryReader::New(stream);
            luabridge::push(L, BinaryReaderWrapper(binary_reader));
            return 1;
        }

        //---------------------------------------------------------
        int io_newBinaryWriter(lua_State* L)
        {
            io::Stream* stream = StreamWrapper::Get(L, 1);
            io::BinaryWriter::Ptr binary_writer = io::BinaryWriter::New(stream);
            luabridge::push(L, BinaryWriterWrapper(binary_writer));
            return 1;
        }

        //---------------------------------------------------------
        bool get_open_mode_constant(const std::string& open_mode_str, int& out_open_mode)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("in",     io::Stream::In    )
                ("out",    io::Stream::Out   )
                ("append", io::Stream::Append);

            map_type::iterator mapped_value = map.find(open_mode_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_open_mode = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        int io_openFile(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            const char* open_mode_str = luaL_optstring(L, 2, "in");
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            int open_mode;
            if (!get_open_mode_constant(open_mode_str, open_mode)) {
                return luaL_error(L, "invalid open mode constant '%s'", open_mode_str);
            }
            io::FileStream::Ptr file = io::OpenFile(filename, (io::Stream::OpenMode)open_mode);
            if (file) {
                luabridge::push(L, FileStreamWrapper(file));
            } else {
                lua_pushnil(L);
            }
            return 1;
        }

        //---------------------------------------------------------
        int io_makeDirectory(lua_State* L)
        {
            const char* dirname = luaL_checkstring(L, 1);
            if (std::strlen(dirname) == 0) {
                return luaL_error(L, "empty directory name");
            }
            lua_pushboolean(L, io::MakeDirectory(dirname));
            return 1;
        }

        //---------------------------------------------------------
        int io_remove(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushboolean(L, io::Remove(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_enumerate(lua_State* L)
        {
            const char* dirname = luaL_checkstring(L, 1);
            if (std::strlen(dirname) == 0) {
                return luaL_error(L, "empty directory name");
            }
            std::vector<std::string> file_list;
            if (io::Enumerate(dirname, file_list)) {
                luabridge::LuaRef t = luabridge::newTable(L);
                for (size_t i = 0; i < file_list.size(); i++) {
                    t[i+1] = file_list[i];
                }
                luabridge::push(L, t);
            } else {
                lua_pushnil(L);
            }
            return 1;
        }

        //---------------------------------------------------------
        int io_exists(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushboolean(L, io::Exists(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_lastWriteTime(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushnumber(L, io::LastWriteTime(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_isDirectory(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushboolean(L, io::IsDirectory(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_isFile(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushboolean(L, io::IsFile(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_mount(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushboolean(L, io::Mount(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_unmount(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            lua_pushboolean(L, io::Unmount(filename));
            return 1;
        }

        //---------------------------------------------------------
        int io_compress(lua_State* L)
        {
            ByteArray* data = ByteArrayWrapper::Get(L, 1);
            if (data->getSize() == 0) {
                return luaL_error(L, "empty data");
            }
            io::MemoryStream::Ptr output_stream = io::MemoryStream::New();
            if (io::Compress(data->getBuffer(), data->getSize(), output_stream)) {
                ByteArray::Ptr compressed_data = ByteArray::New(output_stream->getBuffer(), output_stream->getSize());
                luabridge::push(L, ByteArrayWrapper(compressed_data));
            } else {
                lua_pushnil(L);
            }
            return 1;
        }

        //---------------------------------------------------------
        int io_decompress(lua_State* L)
        {
            ByteArray* compressed_data = ByteArrayWrapper::Get(L, 1);
            if (compressed_data->getSize() == 0) {
                return luaL_error(L, "empty compressed data");
            }
            io::MemoryStream::Ptr output_stream = io::MemoryStream::New();
            if (io::Decompress(compressed_data->getBuffer(), compressed_data->getSize(), output_stream)) {
                ByteArray::Ptr data = ByteArray::New(output_stream->getBuffer(), output_stream->getSize());
                luabridge::push(L, ByteArrayWrapper(data));
            } else {
                lua_pushnil(L);
            }
            return 1;
        }

        /***********************************************************
         *                       INPUT MODULE
         **********************************************************/

        //---------------------------------------------------------
        bool get_rpg_key_constant(int rpg_key, std::string& out_rpg_key_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::KEY_DOWN,     "down"    )
                (RPG::KEY_LEFT,     "left"    )
                (RPG::KEY_RIGHT,    "right"   )
                (RPG::KEY_UP,       "up"      )
                (RPG::KEY_DECISION, "decision")
                (RPG::KEY_CANCEL,   "cancel"  )
                (RPG::KEY_0,        "0"       )
                (RPG::KEY_1,        "1"       )
                (RPG::KEY_2,        "2"       )
                (RPG::KEY_3,        "3"       )
                (RPG::KEY_4,        "4"       )
                (RPG::KEY_5,        "5"       )
                (RPG::KEY_6,        "6"       )
                (RPG::KEY_7,        "7"       )
                (RPG::KEY_8,        "8"       )
                (RPG::KEY_9,        "9"       )
                (RPG::KEY_ADD,      "add"     )
                (RPG::KEY_SUBTRACT, "subtract")
                (RPG::KEY_MULTIPLY, "multiply")
                (RPG::KEY_DIVIDE,   "divide"  )
                (RPG::KEY_DECIMAL,  "decimal" );

            map_type::iterator mapped_value = map.find(rpg_key);

            if (mapped_value == map.end()) {
                return false;
            }

            out_rpg_key_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_rpg_key_constant(const std::string& rpg_key_str, int& out_rpg_key)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("down",     RPG::KEY_DOWN    )
                ("left",     RPG::KEY_LEFT    )
                ("right",    RPG::KEY_RIGHT   )
                ("up",       RPG::KEY_UP      )
                ("decision", RPG::KEY_DECISION)
                ("cancel",   RPG::KEY_CANCEL  )
                ("0",        RPG::KEY_0       )
                ("1",        RPG::KEY_1       )
                ("2",        RPG::KEY_2       )
                ("3",        RPG::KEY_3       )
                ("4",        RPG::KEY_4       )
                ("5",        RPG::KEY_5       )
                ("6",        RPG::KEY_6       )
                ("7",        RPG::KEY_7       )
                ("8",        RPG::KEY_8       )
                ("9",        RPG::KEY_9       )
                ("add",      RPG::KEY_ADD     )
                ("subtract", RPG::KEY_SUBTRACT)
                ("multiply", RPG::KEY_MULTIPLY)
                ("divide",   RPG::KEY_DIVIDE  )
                ("decimal",  RPG::KEY_DECIMAL );

            map_type::iterator mapped_value = map.find(rpg_key_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_rpg_key = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        int input_isDown(lua_State* L)
        {
            int nargs = lua_gettop(L);
            if (nargs < 1) {
                luaL_error(L, "insufficient number of arguments (expected at least one)");
                return 0;
            }

            bool result = false;
            for (int i = 1; i <= nargs; i++) {
                const char* rpg_key_str = luaL_checkstring(L, i);
                int rpg_key;
                if (!get_rpg_key_constant(rpg_key_str, rpg_key)) {
                    luaL_error(L, "bad argument #%d (invalid rpg key constant '%s')", i, rpg_key_str);
                    return 0;
                }
                result |= RPG::input->pressed((RPG::Key)rpg_key);
            }
            lua_pushboolean(L, result);

            return 1;
        }

        //---------------------------------------------------------
        int input_getKeyCodes(lua_State* L)
        {
            const char* rpg_key_str = luaL_checkstring(L, 1);

            int rpg_key;
            if (!get_rpg_key_constant(rpg_key_str, rpg_key)) {
                const char* msg = lua_pushfstring(L, "invalid rpg key constant '%s'", rpg_key_str);
                return luaL_argerror(L, 1, msg);
            }

            int* key_codes = RPG::input->key((RPG::Key)rpg_key);

            lua_createtable(L, 8, 0);
            for (int i = 0; i < 8; i++) {
                lua_pushnumber(L, key_codes[i]);
                lua_rawseti(L, -2, i+1);
            }

            return 1;
        }

        //---------------------------------------------------------
        int input_setKeyCodes(lua_State* L)
        {
            const char* rpg_key_str = luaL_checkstring(L, 1);

            int rpg_key;
            if (!get_rpg_key_constant(rpg_key_str, rpg_key)) {
                const char* msg = lua_pushfstring(L, "invalid rpg key constant '%s'", rpg_key_str);
                return luaL_argerror(L, 2, msg);
            }

            if (lua_gettop(L) < 2) {
                return luaL_argerror(L, 2, "expected table, got nothing");
            }

            if (!lua_istable(L, 2)) {
                const char* msg = lua_pushfstring(L, "expected table, got %s", lua_typename(L, lua_type(L, 2)));
                return luaL_argerror(L, 2, msg);
            }

            int* key_codes = RPG::input->key((RPG::Key)rpg_key);

            for (int i = 0; i < 8; i++) {
                lua_rawgeti(L, 2, i+1); // get number

                if (!lua_isnumber(L, -1)) {
                    const char* msg = lua_pushfstring(L, "expected number at index %d, got %s", i+1, lua_typename(L, lua_type(L, -1)));
                    return luaL_argerror(L, 2, msg);
                }

                key_codes[i] = (int)lua_tonumber(L, -1);

                lua_pop(L, 1); // pop number
            }

            return 0;
        }

        /***********************************************************
         *                      AUDIO MODULE
         **********************************************************/

        //---------------------------------------------------------
        int audio_openSound(lua_State* L)
        {
            if (lua_isstring(L, 1))
            {
                // openSound(filename)
                const char* filename = lua_tostring(L, 1);
                audio::Sound::Ptr sound = audio::OpenSound(filename);
                if (sound) {
                    luabridge::push(L, SoundWrapper(sound));
                } else {
                    lua_pushnil(L);
                }
            }
            else
            {
                // openSound(stream)
                io::Stream* stream = StreamWrapper::Get(L, 1);
                audio::Sound::Ptr sound = audio::OpenSound(stream);
                if (sound) {
                    luabridge::push(L, SoundWrapper(sound));
                } else {
                    lua_pushnil(L);
                }
            }
            return 1;
        }

        //---------------------------------------------------------
        int audio_openSoundEffect(lua_State* L)
        {
            if (lua_isstring(L, 1))
            {
                // openSoundEffect(filename)
                const char* filename = lua_tostring(L, 1);
                audio::SoundEffect::Ptr se = audio::OpenSoundEffect(filename);
                if (se) {
                    luabridge::push(L, SoundEffectWrapper(se));
                } else {
                    lua_pushnil(L);
                }
            }
            else
            {
                // openSoundEffect(stream)
                io::Stream* stream = StreamWrapper::Get(L, 1);
                audio::SoundEffect::Ptr sound_effect = audio::OpenSoundEffect(stream);
                if (sound_effect) {
                    luabridge::push(L, SoundEffectWrapper(sound_effect));
                } else {
                    lua_pushnil(L);
                }
            }
            return 1;
        }

        //---------------------------------------------------------
        typedef RPG::Music* (RPG::System::*system_bgm_getter)();
        typedef RPG::Sound* (RPG::System::*system_se_getter)();

        //---------------------------------------------------------
        bool get_system_bgm_getter(const std::string& system_bgm_str, system_bgm_getter& out_get_system_bgm)
        {
            typedef boost::unordered_map<std::string, system_bgm_getter> map_type;

            static map_type map = boost::assign::map_list_of
                ("title screen",     &RPG::System::getTitleBGM   )
                ("battle",           &RPG::System::getBattleBGM  )
                ("victory",          &RPG::System::getVictoryBGM )
                ("inn",              &RPG::System::getInnBGM     )
                ("skiff",            &RPG::System::getSkiffBGM   )
                ("ship",             &RPG::System::getShipBGM    )
                ("airship",          &RPG::System::getAirshipBGM )
                ("game over screen", &RPG::System::getGameOverBGM);

            map_type::iterator mapped_value = map.find(system_bgm_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_get_system_bgm = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_system_se_getter(const std::string& system_se_str, system_se_getter& out_get_system_se)
        {
            typedef boost::unordered_map<std::string, system_se_getter> map_type;

            static map_type map = boost::assign::map_list_of
                ("cursor",            &RPG::System::getCursorSE     )
                ("select choice",     &RPG::System::getDecisionSE   )
                ("cancel",            &RPG::System::getCancelSE     )
                ("buzzer",            &RPG::System::getBuzzerSE     )
                ("battle transition", &RPG::System::getBattleStartSE)
                ("flee battle",       &RPG::System::getFleeSE       )
                ("enemy attacks",     &RPG::System::getEnemyAttackSE)
                ("enemy damaged",     &RPG::System::getEnemyDamageSE)
                ("ally damaged",      &RPG::System::getHeroDamageSE )
                ("evade attack",      &RPG::System::getEvasionSE    )
                ("enemy defeated",    &RPG::System::getEnemyDeathSE )
                ("item used",         &RPG::System::getItemSE       );

            map_type::iterator mapped_value = map.find(system_se_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_get_system_se = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        int audio_playSystemBGM(lua_State* L)
        {
            const char* system_bgm_str = luaL_checkstring(L, 1);

            system_bgm_getter get_system_bgm;
            if (!get_system_bgm_getter(system_bgm_str, get_system_bgm)) {
                return luaL_error(L, "invalid system bgm constant");
            }
            (RPG::system->*get_system_bgm)()->play();

            return 0;
        }

        //---------------------------------------------------------
        int audio_playBGM(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            int fade_in_time     = luaL_optint(L, 2, 0);
            int volume           = luaL_optint(L, 3, 100);
            int tempo            = luaL_optint(L, 4, 100);
            int pan              = luaL_optint(L, 5, 50);

            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            if (fade_in_time < 0) {
                return luaL_error(L, "invalid fade in time");
            }
            if (volume < 0 || volume > 100) {
                return luaL_error(L, "invalid volume");
            }
            if (tempo < 50 || tempo > 150) {
                return luaL_error(L, "invalid tempo");
            }
            if (pan < 0 || pan > 100) {
                return luaL_error(L, "invalid pan");
            }

            RPG::Music bgm(filename, fade_in_time, volume, tempo, pan);
            bgm.play();

            return 0;
        }

        //---------------------------------------------------------
        int audio_fadeBGM(lua_State* L)
        {
            int fade_out_time = luaL_checkint(L, 1);
            RPG::Music::fadeOut(fade_out_time);
            return 0;
        }

        //---------------------------------------------------------
        int audio_stopBGM(lua_State* L)
        {
            RPG::Music::stop();
            return 0;
        }

        //---------------------------------------------------------
        int audio_playSystemSE(lua_State* L)
        {
            const char* system_se_str = luaL_checkstring(L, 1);

            system_se_getter get_system_se;
            if (!get_system_se_getter(system_se_str, get_system_se)) {
                return luaL_error(L, "invalid system se constant");
            }
            (RPG::system->*get_system_se)()->play();

            return 0;
        }

        //---------------------------------------------------------
        int audio_playSE(lua_State* L)
        {
            const char* filename = luaL_checkstring(L, 1);
            int volume           = luaL_optint(L, 2, 100);
            int tempo            = luaL_optint(L, 3, 100);
            int pan              = luaL_optint(L, 4, 50);

            if (std::strlen(filename) == 0) {
                return luaL_error(L, "empty filename");
            }
            if (volume < 0 || volume > 100) {
                return luaL_error(L, "invalid volume");
            }
            if (tempo < 50 || tempo > 150) {
                return luaL_error(L, "invalid tempo");
            }
            if (pan < 0 || pan > 100) {
                return luaL_error(L, "invalid pan");
            }

            RPG::Sound se(filename, volume, tempo, pan);
            se.play();

            return 0;
        }

        //---------------------------------------------------------
        int audio_stopSE(lua_State* L)
        {
            RPG::Sound::stop();
            return 0;
        }

        /***********************************************************
         *                     GRAPHICS MODULE
         **********************************************************/

        //---------------------------------------------------------
        struct Graphics
        {
            //---------------------------------------------------------
            static u16* Pixels()
            {
                return RPG::screen->canvas->getScanline(0);
            }

            //---------------------------------------------------------
            static int Width()
            {
                return RPG::screen->canvas->width();
            }

            //---------------------------------------------------------
            static int Height()
            {
                return RPG::screen->canvas->height();
            }

            //---------------------------------------------------------
            static int Pitch()
            {
                return RPG::screen->canvas->lineSize / 2;
            }

            static int BlendMode;
            static Recti ClipRect;
        };

        //---------------------------------------------------------
        int   Graphics::BlendMode = graphics::BlendMode::Mix;
        Recti Graphics::ClipRect  = Recti(0, 0, 320, 240);

        //---------------------------------------------------------
        int graphics_get_width()
        {
            return RPG::screen->canvas->width();
        }

        //---------------------------------------------------------
        int graphics_get_height()
        {
            return RPG::screen->canvas->height();
        }

        //---------------------------------------------------------
        int graphics_get_brightness()
        {
            return RPG::screen->canvas->brightness;
        }

        //---------------------------------------------------------
        void graphics_set_brightness(int brightness)
        {
            RPG::screen->canvas->brightness = brightness;
        }

        //---------------------------------------------------------
        int graphics_getDimensions(lua_State* L)
        {
            lua_pushnumber(L, RPG::screen->canvas->width());
            lua_pushnumber(L, RPG::screen->canvas->height());
            return 2;
        }

        //---------------------------------------------------------
        int graphics_getBlendMode(lua_State* L)
        {
            std::string blend_mode_str;
            if (!get_blend_mode_constant(Graphics::BlendMode, blend_mode_str)) {
                throw "unexpected internal value";
            }
            lua_pushstring(L, blend_mode_str.c_str());
            return 1;
        }

        //---------------------------------------------------------
        int graphics_setBlendMode(lua_State* L)
        {
            const char* blend_mode_str = luaL_checkstring(L, 1);
            int blend_mode;
            if (!get_blend_mode_constant(blend_mode_str, blend_mode)) {
                throw "invalid value";
            }
            Graphics::BlendMode = blend_mode;
            return 0;
        }

        //---------------------------------------------------------
        int graphics_getClipRect(lua_State* L)
        {
            lua_pushnumber(L, Graphics::ClipRect.getX());
            lua_pushnumber(L, Graphics::ClipRect.getY());
            lua_pushnumber(L, Graphics::ClipRect.getWidth());
            lua_pushnumber(L, Graphics::ClipRect.getHeight());
            return 4;
        }

        //---------------------------------------------------------
        int graphics_setClipRect(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);
            int w = luaL_checkint(L, 3);
            int h = luaL_checkint(L, 4);

            Recti clip_rect = Recti(x, y, w, h);
            Recti screen_bounds = Recti(
                RPG::screen->canvas->width(),
                RPG::screen->canvas->height()
            );

            if (!clip_rect.isValid() || !clip_rect.isInside(screen_bounds)) {
                throw "invalid rect";
            }

            Graphics::ClipRect = clip_rect;
            return 0;
        }

        //---------------------------------------------------------
        int graphics_packColor(lua_State* L)
        {
            u8 r = luaL_checkunsigned(L, 1);
            u8 g = luaL_checkunsigned(L, 2);
            u8 b = luaL_checkunsigned(L, 3);
            u8 a = luaL_optunsigned(L, 4, 255);
            lua_pushunsigned(L, graphics::RGBA8888(r, g, b, a));
            return 1;
        }

        //---------------------------------------------------------
        int graphics_unpackColor(lua_State* L)
        {
            u32 c = luaL_checkunsigned(L, 1);

            graphics::RGBA unpacked = graphics::RGBA8888ToRGBA(c);
            lua_pushunsigned(L, unpacked.red);
            lua_pushunsigned(L, unpacked.green);
            lua_pushunsigned(L, unpacked.blue);
            lua_pushunsigned(L, unpacked.alpha);

            return 4;
        }

        //---------------------------------------------------------
        int graphics_copyRect(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);
            int w = luaL_checkint(L, 3);
            int h = luaL_checkint(L, 4);
            graphics::Image* destination = ImageWrapper::GetOpt(L, 5);

            Recti rect(x, y, w, h);
            Recti screen_bounds(RPG::screen->canvas->width(), RPG::screen->canvas->height());

            if (!rect.isValid() || !rect.isInside(screen_bounds)) {
                return luaL_error(L, "invalid rect");
            }

            graphics::Image::Ptr image;

            if (destination) {
                image = destination;
                image->resize(w, h);
            } else {
                image = graphics::Image::New(w, h);
            }

            graphics::RGBA* dst = image->getPixels();

            int  src_pitch = Graphics::Pitch();
            u16* src       = Graphics::Pixels() + src_pitch * y + x;

            for (int iy = 0; iy < h; iy++) {
                for (int ix = 0; ix < w; ix++) {
                    *dst = graphics::RGB565ToRGBA(*src);
                    src++;
                    dst++;
                }
                src += src_pitch - w;
            }

            luabridge::push(L, ImageWrapper(image));
            return 1;
        }

        //---------------------------------------------------------
        void graphics_clear_generic(u16 color)
        {
            u16* dp = Graphics::Pixels();
            int  di = Graphics::Pitch() - Graphics::Width();

            for (int y = Graphics::Height(); y > 0; y--) {
                for (int x = Graphics::Width(); x > 0; x--) {
                    *dp = color;
                    dp++;
                }
                dp += di;
            }
        }

        //---------------------------------------------------------
        void graphics_clear_sse2(u16 color)
        {
            u16* dp = Graphics::Pixels();
            int  di = Graphics::Pitch() - Graphics::Width();

            int burst1 = RPG::screen->canvas->width() / 8;
            int burst2 = RPG::screen->canvas->width() % 8;

            __m128i mcolor = _mm_set_epi16(
                color, color, color, color,
                color, color, color, color
            );

            for (int y = Graphics::Height(); y > 0; y--) {
                for (int x = burst1; x > 0; x--) {
                    _mm_storeu_si128((__m128i*)dp, mcolor);
                    dp += 8;
                }
                for (int x = burst2; x > 0; x--) {
                    *dp = color;
                    dp++;
                }
                dp += di;
            }
        }

        //---------------------------------------------------------
        int graphics_clear(lua_State* L)
        {
            u32 c = luaL_optunsigned(L, 1, 0x000000FF);

            if (graphics::CPUSupportsSSE2())
            {
                graphics_clear_sse2(graphics::RGBA8888ToRGB565(c));
            }
            else
            {
                graphics_clear_generic(graphics::RGBA8888ToRGB565(c));
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_grey(lua_State* L)
        {
            int  dst_pitch  = Graphics::Pitch();
            u16* dst_pixels = Graphics::Pixels();

            int w = RPG::screen->canvas->width();
            int h = RPG::screen->canvas->height();

            for (int y = 0; y < h; y++) {
                u16* dst = dst_pixels + dst_pitch * y;
                for (int x = 0; x < w; x++) {
                    unsigned int c = *dst;
                    unsigned int g = ((c >> 11) + ((c >> 6) & 0x001F) + (c & 0x001F)) / 3;
                    *dst = (g << 11) | ((g << 6) | 0x01)  | g;
                    dst++;
                }
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_getPixel(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);

            luaL_argcheck(L, x >= 0 && x < RPG::screen->canvas->width(),  1, "invalid x");
            luaL_argcheck(L, y >= 0 && y < RPG::screen->canvas->height(), 2, "invalid y");

            u16 c = *(Graphics::Pixels() + Graphics::Pitch() * y + x);
            lua_pushunsigned(L, graphics::RGB565ToRGBA8888(c));

            return 1;
        }

        //---------------------------------------------------------
        int graphics_setPixel(lua_State* L)
        {
            int x = luaL_checkint(L, 1);
            int y = luaL_checkint(L, 2);
            u32 c = luaL_checkunsigned(L, 3);

            luaL_argcheck(L, x >= 0 && x < RPG::screen->canvas->width(),  1, "invalid x");
            luaL_argcheck(L, y >= 0 && y < RPG::screen->canvas->height(), 2, "invalid y");

            *(Graphics::Pixels() + Graphics::Pitch() * y + x) = graphics::RGBA8888ToRGB565(c);

            return 0;
        }

        //---------------------------------------------------------
        struct rgb565_set
        {
            void operator()(u16& dst, const graphics::RGBA& src)
            {
                dst = ((src.red & 0xF8) << 8) | ((src.green & 0xFC) << 3) | (src.blue >> 3);
            }

            void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
            {
                dst = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
            }
        };

        //---------------------------------------------------------
        struct rgb565_mix
        {
            void operator()(u16& dst, const graphics::RGBA& src)
            {
                graphics::RGBA temp;
                int sa = src.alpha + 1;
                int da = 256 - src.alpha;
                temp.red   = (   (dst >>  11)         * da + (src.red   >> 3) * sa ) >> 8;
                temp.green = ( ( (dst >>   5) & 0x3F) * da + (src.green >> 2) * sa ) >> 8;
                temp.blue  = ( (  dst         & 0x1F) * da + (src.blue  >> 3) * sa ) >> 8;
                dst = (temp.red << 11) | (temp.green << 5) | temp.blue;
            }

            void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
            {
                graphics::RGBA temp;
                int sa = alpha + 1;
                int da = 256 - alpha;
                temp.red   = (   (dst >> 11)         * da + (red   >> 3) * sa ) >> 8;
                temp.green = ( ( (dst >>  5) & 0x3F) * da + (green >> 2) * sa ) >> 8;
                temp.blue  = ( (  dst        & 0x1F) * da + (blue  >> 3) * sa ) >> 8;
                dst = (temp.red << 11) | (temp.green << 5) | temp.blue;
            }
        };

        //---------------------------------------------------------
        struct rgb565_add
        {
            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r = std::min(  (dst >> 11)         + (src.red   >> 3), 31);
                u8 g = std::min(( (dst >>  5) & 0x3F) + (src.green >> 2), 63);
                u8 b = std::min((  dst        & 0x1F) + (src.blue  >> 3), 31);
                dst = (r << 11) | (g << 5) | b;
            }

            void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
            {
                u8 r = std::min(  (dst >> 11)         + (red   >> 3), 31);
                u8 g = std::min(( (dst >>  5) & 0x3F) + (green >> 2), 63);
                u8 b = std::min((  dst        & 0x1F) + (blue  >> 3), 31);
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        struct rgb565_sub
        {
            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r = std::max(  (dst >> 11)         - (src.red   >> 3), 0);
                u8 g = std::max(( (dst >>  5) & 0x3F) - (src.green >> 2), 0);
                u8 b = std::max((  dst        & 0x1F) - (src.blue  >> 3), 0);
                dst = (r << 11) | (g << 5) | b;
            }

            void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
            {
                u8 r = std::max(  (dst >> 11)         - (red   >> 3), 0);
                u8 g = std::max(( (dst >>  5) & 0x3F) - (green >> 2), 0);
                u8 b = std::max((  dst        & 0x1F) - (blue  >> 3), 0);
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        struct rgb565_mul
        {
            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r =   (dst >> 11)         * ((src.red   >> 3) + 1) >> 5;
                u8 g = ( (dst >>  5) & 0x3F) * ((src.green >> 2) + 1) >> 6;
                u8 b = (  dst        & 0x1F) * ((src.blue  >> 3) + 1) >> 5;
                dst = (r << 11) | (g << 5) | b;
            }

            void operator()(u16& dst, u8 red, u8 green, u8 blue, u8 alpha)
            {
                u8 r =   (dst >> 11)         * ((red   >> 3) + 1) >> 5;
                u8 g = ( (dst >>  5) & 0x3F) * ((green >> 2) + 1) >> 6;
                u8 b = (  dst        & 0x1F) * ((blue  >> 3) + 1) >> 5;
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        int graphics_drawPoint(lua_State* L)
        {
            int x = luaL_checkint(L, 2);
            int y = luaL_checkint(L, 3);
            u32 c = luaL_checkunsigned(L, 4);

            switch (Graphics::BlendMode) {
            case graphics::BlendMode::Set:      graphics::primitives::Point(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_set()); break;
            case graphics::BlendMode::Mix:      graphics::primitives::Point(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_mix()); break;
            case graphics::BlendMode::Add:      graphics::primitives::Point(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_add()); break;
            case graphics::BlendMode::Subtract: graphics::primitives::Point(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_sub()); break;
            case graphics::BlendMode::Multiply: graphics::primitives::Point(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x, y), graphics::RGBA8888ToRGBA(c), rgb565_mul()); break;
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_drawLine(lua_State* L)
        {
            int x1 = luaL_checkint(L, 1);
            int y1 = luaL_checkint(L, 2);
            int x2 = luaL_checkint(L, 3);
            int y2 = luaL_checkint(L, 4);
            u32 c1 = luaL_checkunsigned(L, 5);
            u32 c2 = luaL_optunsigned(L, 6, c1);

            switch (Graphics::BlendMode) {
            case graphics::BlendMode::Set:      graphics::primitives::Line(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x1, y1), Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_set()); break;
            case graphics::BlendMode::Mix:      graphics::primitives::Line(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x1, y1), Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mix()); break;
            case graphics::BlendMode::Add:      graphics::primitives::Line(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x1, y1), Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_add()); break;
            case graphics::BlendMode::Subtract: graphics::primitives::Line(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x1, y1), Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_sub()); break;
            case graphics::BlendMode::Multiply: graphics::primitives::Line(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Vec2i(x1, y1), Vec2i(x2, y2), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mul()); break;
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_drawRectangle(lua_State* L)
        {
            bool fill = lua_toboolean(L, 1);
            int     x = luaL_checkint(L, 2);
            int     y = luaL_checkint(L, 3);
            int     w = luaL_checkint(L, 4);
            int     h = luaL_checkint(L, 5);
            u32    c1 = luaL_checkunsigned(L, 6);
            u32    c2 = luaL_optunsigned(L, 7, c1);
            u32    c3 = luaL_optunsigned(L, 8, c1);
            u32    c4 = luaL_optunsigned(L, 9, c1);

            switch (Graphics::BlendMode) {
            case graphics::BlendMode::Set:      graphics::primitives::Rectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_set()); break;
            case graphics::BlendMode::Mix:      graphics::primitives::Rectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_mix()); break;
            case graphics::BlendMode::Add:      graphics::primitives::Rectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_add()); break;
            case graphics::BlendMode::Subtract: graphics::primitives::Rectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_sub()); break;
            case graphics::BlendMode::Multiply: graphics::primitives::Rectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Recti(x, y, w, h), graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), graphics::RGBA8888ToRGBA(c3), graphics::RGBA8888ToRGBA(c4), rgb565_mul()); break;
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_drawCircle(lua_State* L)
        {
            bool fill = lua_toboolean(L, 1);
            int     x = luaL_checkint(L, 2);
            int     y = luaL_checkint(L, 3);
            int     r = luaL_checkint(L, 4);
            u32    c1 = luaL_checkunsigned(L, 5);
            u32    c2 = luaL_optunsigned(L, 6, c1);

            switch (Graphics::BlendMode) {
            case graphics::BlendMode::Set:      graphics::primitives::Circle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_set()); break;
            case graphics::BlendMode::Mix:      graphics::primitives::Circle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mix()); break;
            case graphics::BlendMode::Add:      graphics::primitives::Circle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_add()); break;
            case graphics::BlendMode::Subtract: graphics::primitives::Circle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_sub()); break;
            case graphics::BlendMode::Multiply: graphics::primitives::Circle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, fill, Vec2i(x, y), r, graphics::RGBA8888ToRGBA(c1), graphics::RGBA8888ToRGBA(c2), rgb565_mul()); break;
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_drawTriangle(lua_State* L)
        {
            /*
            bool fill = lua_toboolean(L, 1);
            int    x1 = luaL_checkint(L, 2);
            int    y1 = luaL_checkint(L, 3);
            int    x2 = luaL_checkint(L, 4);
            int    y2 = luaL_checkint(L, 5);
            int    x3 = luaL_checkint(L, 6);
            int    y3 = luaL_checkint(L, 7);
            u32    c1 = luaL_checkunsigned(L, 8);
            u32    c2 = luaL_optunsigned(L, 9, c1);
            u32    c3 = luaL_optunsigned(L, 10, c1);
            */

            // TODO

            return 0;
        }

        //---------------------------------------------------------
        struct rgb565_set_col
        {
            graphics::RGBA c;

            explicit rgb565_set_col(graphics::RGBA color)
                : c(color)
            {
                c.red   = (c.red   >> 3) + 1;
                c.green = (c.green >> 2) + 1;
                c.blue  = (c.blue  >> 3) + 1;
            }

            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r = ((src.red   >> 3) * c.red  ) >> 5;
                u8 g = ((src.green >> 2) * c.green) >> 6;
                u8 b = ((src.blue  >> 3) * c.blue ) >> 5;
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        struct rgb565_mix_col
        {
            graphics::RGBA c;

            explicit rgb565_mix_col(graphics::RGBA color)
                : c(color)
            {
                c.red   = (c.red   >> 3) + 1;
                c.green = (c.green >> 2) + 1;
                c.blue  = (c.blue  >> 3) + 1;
            }

            void operator()(u16& dst, const graphics::RGBA& src)
            {
                int sa = (src.alpha * (c.alpha + 1) >> 8) + 1;
                int da = 256 - (sa - 1);
                u8 r = (   (dst >>  11)         * da + (((src.red   >> 3) * c.red  ) >> 5) * sa ) >> 8;
                u8 g = ( ( (dst >>   5) & 0x3F) * da + (((src.green >> 2) * c.green) >> 6) * sa ) >> 8;
                u8 b = ( (  dst         & 0x1F) * da + (((src.blue  >> 3) * c.blue ) >> 5) * sa ) >> 8;
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        struct rgb565_add_col
        {
            graphics::RGBA c;

            explicit rgb565_add_col(graphics::RGBA color)
                : c(color)
            {
                c.red   = (c.red   >> 3) + 1;
                c.green = (c.green >> 2) + 1;
                c.blue  = (c.blue  >> 3) + 1;
            }

            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r = std::min(   (dst >> 11)         + (((src.red   >> 3) * c.red  ) >> 5), 31 );
                u8 g = std::min( ( (dst >>  5) & 0x3F) + (((src.green >> 2) * c.green) >> 6), 63 );
                u8 b = std::min( (  dst        & 0x1F) + (((src.blue  >> 3) * c.blue ) >> 5), 31 );
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        struct rgb565_sub_col
        {
            graphics::RGBA c;

            explicit rgb565_sub_col(graphics::RGBA color)
                : c(color)
            {
                c.red   = (c.red   >> 3) + 1;
                c.green = (c.green >> 2) + 1;
                c.blue  = (c.blue  >> 3) + 1;
            }

            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r = std::max(   (dst >> 11)         - (((src.red   >> 3) * c.red  ) >> 5), 0 );
                u8 g = std::max( ( (dst >>  5) & 0x3F) - (((src.green >> 2) * c.green) >> 6), 0 );
                u8 b = std::max( (  dst        & 0x1F) - (((src.blue  >> 3) * c.blue ) >> 5), 0 );
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        struct rgb565_mul_col
        {
            graphics::RGBA c;

            explicit rgb565_mul_col(graphics::RGBA color)
                : c(color)
            {
                c.red   = (c.red   >> 3) + 1;
                c.green = (c.green >> 2) + 1;
                c.blue  = (c.blue  >> 3) + 1;
            }

            void operator()(u16& dst, const graphics::RGBA& src)
            {
                u8 r =   (dst >> 11)         * ((((src.red   >> 3) * c.red  ) >> 5) + 1) >> 5;
                u8 g = ( (dst >>  5) & 0x3F) * ((((src.green >> 2) * c.green) >> 6) + 1) >> 6;
                u8 b = (  dst        & 0x1F) * ((((src.blue  >> 3) * c.blue ) >> 5) + 1) >> 5;
                dst = (r << 11) | (g << 5) | b;
            }
        };

        //---------------------------------------------------------
        int graphics_draw(lua_State* L)
        {
            graphics::Image* that = 0;
            int   sx, sy, sw, sh;
            int   x, y;
            float rotate;
            float scale;
            u32   color;

            int nargs = lua_gettop(L);
            if (nargs >= 7)
            {
                that = ImageWrapper::Get(L, 1);
                sx   = luaL_checkint(L, 2);
                sy   = luaL_checkint(L, 3);
                sw   = luaL_checkint(L, 4);
                sh   = luaL_checkint(L, 5);
                x    = luaL_checkint(L, 6);
                y    = luaL_checkint(L, 7);
                rotate = luaL_optnumber(L, 8, 0.0);
                scale  = luaL_optnumber(L, 9, 1.0);
                color  = luaL_optunsigned(L, 10, 0xFFFFFFFF);
            }
            else
            {
                that = ImageWrapper::Get(L, 1);
                x    = luaL_checkint(L, 2);
                y    = luaL_checkint(L, 3);
                rotate = luaL_optnumber(L, 4, 0.0);
                scale  = luaL_optnumber(L, 5, 1.0);
                color  = luaL_optunsigned(L, 6, 0xFFFFFFFF);

                sx = 0;
                sy = 0;
                sw = that->getWidth();
                sh = that->getHeight();
            }

            if (rotate == 0.0)
            {
                if (color == 0xFFFFFFFF)
                {
                    switch (Graphics::BlendMode) {
                    case graphics::BlendMode::Set:      graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_set()); break;
                    case graphics::BlendMode::Mix:      graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mix()); break;
                    case graphics::BlendMode::Add:      graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_add()); break;
                    case graphics::BlendMode::Subtract: graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_sub()); break;
                    case graphics::BlendMode::Multiply: graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mul()); break;
                    }
                }
                else
                {
                    switch (Graphics::BlendMode) {
                    case graphics::BlendMode::Set:      graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_set_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Mix:      graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mix_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Add:      graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_add_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Subtract: graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_sub_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Multiply: graphics::primitives::TexturedRectangle(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, Recti(x, y, sw, sh).scale(scale), that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mul_col(graphics::RGBA8888ToRGBA(color))); break;
                    }
                }
            }
            else
            {
                Recti rect = Recti(x, y, sw, sh).scale(scale);
                Vec2i center_of_rotation = rect.getCenter();

                Vec2i vertices[4] = {
                    rect.getUpperLeft().rotateBy(rotate, center_of_rotation),
                    rect.getUpperRight().rotateBy(rotate, center_of_rotation),
                    rect.getLowerRight().rotateBy(rotate, center_of_rotation),
                    rect.getLowerLeft().rotateBy(rotate, center_of_rotation)
                };

                if (color == 0xFFFFFFFF)
                {
                    switch (Graphics::BlendMode) {
                    case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_set()); break;
                    case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mix()); break;
                    case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_add()); break;
                    case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_sub()); break;
                    case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mul()); break;
                    }
                }
                else
                {
                    switch (Graphics::BlendMode) {
                    case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_set_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mix_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_add_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_sub_col(graphics::RGBA8888ToRGBA(color))); break;
                    case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mul_col(graphics::RGBA8888ToRGBA(color))); break;
                    }
                }
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_drawq(lua_State* L)
        {
            graphics::Image* that = 0;
            int sx, sy, sw, sh;
            int x1, y1;
            int x2, y2;
            int x3, y3;
            int x4, y4;
            u32 color;

            int nargs = lua_gettop(L);
            if (nargs >= 13)
            {
                that = ImageWrapper::Get(L, 1);
                sx   = luaL_checkint(L, 2);
                sy   = luaL_checkint(L, 3);
                sw   = luaL_checkint(L, 4);
                sh   = luaL_checkint(L, 5);
                x1   = luaL_checkint(L, 6);
                y1   = luaL_checkint(L, 7);
                x2   = luaL_checkint(L, 8);
                y2   = luaL_checkint(L, 9);
                x3   = luaL_checkint(L, 10);
                y3   = luaL_checkint(L, 11);
                x4   = luaL_checkint(L, 12);
                y4   = luaL_checkint(L, 13);
                color = luaL_optunsigned(L, 14, 0xFFFFFFFF);
            }
            else
            {
                that = ImageWrapper::Get(L, 1);
                x1   = luaL_checkint(L, 2);
                y1   = luaL_checkint(L, 3);
                x2   = luaL_checkint(L, 4);
                y2   = luaL_checkint(L, 5);
                x3   = luaL_checkint(L, 6);
                y3   = luaL_checkint(L, 7);
                x4   = luaL_checkint(L, 8);
                y4   = luaL_checkint(L, 9);
                color = luaL_optunsigned(L, 10, 0xFFFFFFFF);

                sx = 0;
                sy = 0;
                sw = that->getWidth();
                sh = that->getHeight();
            }

            Vec2i vertices[4] = {
                Vec2i(x1, y1),
                Vec2i(x2, y2),
                Vec2i(x3, y3),
                Vec2i(x4, y4)
            };

            if (color == 0xFFFFFFFF)
            {
                switch (Graphics::BlendMode) {
                case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_set()); break;
                case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mix()); break;
                case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_add()); break;
                case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_sub()); break;
                case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mul()); break;
                }
            }
            else
            {
                switch (Graphics::BlendMode) {
                case graphics::BlendMode::Set:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_set_col(graphics::RGBA8888ToRGBA(color))); break;
                case graphics::BlendMode::Mix:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mix_col(graphics::RGBA8888ToRGBA(color))); break;
                case graphics::BlendMode::Add:      graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_add_col(graphics::RGBA8888ToRGBA(color))); break;
                case graphics::BlendMode::Subtract: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_sub_col(graphics::RGBA8888ToRGBA(color))); break;
                case graphics::BlendMode::Multiply: graphics::primitives::TexturedQuad(Graphics::Pixels(), Graphics::Pitch(), Graphics::ClipRect, vertices, that->getPixels(), that->getWidth(), Recti(sx, sy, sw, sh), rgb565_mul_col(graphics::RGBA8888ToRGBA(color))); break;
                }
            }

            return 0;
        }

        //---------------------------------------------------------
        int graphics_newImage(lua_State* L)
        {
            int nargs = lua_gettop(L);
            if (nargs >= 3 && luabridge::Stack<ByteArrayWrapper*>::is_a(L, 3))
            {
                // newImage(width, height, pixels)
                int w = luaL_checkint(L, 1);
                int h = luaL_checkint(L, 2);
                ByteArray* pixels = ByteArrayWrapper::Get(L, 3);

                luaL_argcheck(L, w > 0, 1, "invalid width");
                luaL_argcheck(L, h > 0, 2, "invalid height");
                luaL_argcheck(L, pixels->getSize() == w * h * 4, 3, "invalid pixels");

                graphics::Image::Ptr image = graphics::Image::New(w, h, (const graphics::RGBA*)pixels->getBuffer());

                luabridge::push(L, ImageWrapper(image));
            }
            else
            {
                // newImage(width, height [, color])
                int w = luaL_checkint(L, 1);
                int h = luaL_checkint(L, 2);
                u32 c = luaL_optunsigned(L, 3, 0x000000FF);

                luaL_argcheck(L, w > 0, 1, "invalid width");
                luaL_argcheck(L, h > 0, 2, "invalid height");

                graphics::Image::Ptr image = graphics::Image::New(w, h, graphics::RGBA8888ToRGBA(c));
                luabridge::push(L, ImageWrapper(image));
            }
            return 1;
        }

        //---------------------------------------------------------
        int graphics_readImage(lua_State* L)
        {
            if (lua_isstring(L, 1))
            {
                // readImage(filename)
                const char* filename = lua_tostring(L, 1);
                graphics::Image::Ptr image = graphics::ReadImage(filename);
                if (image) {
                    luabridge::push(L, ImageWrapper(image));
                } else {
                    lua_pushnil(L);
                }
            }
            else
            {
                // readImage(stream)
                io::Stream* stream = StreamWrapper::Get(L, 1);
                graphics::Image::Ptr image = graphics::ReadImage(stream);
                if (image) {
                    luabridge::push(L, ImageWrapper(image));
                } else {
                    lua_pushnil(L);
                }
            }
            return 1;
        }

        //---------------------------------------------------------
        int graphics_writeImage(lua_State* L)
        {
            int nargs = lua_gettop(L);
            if (nargs >= 2 && lua_isstring(L, 2))
            {
                // writeImage(image, filename)
                graphics::Image* image = ImageWrapper::Get(L, 1);
                const char* filename = luaL_checkstring(L, 2);
                lua_pushboolean(L, graphics::WriteImage(image, filename));
            }
            else
            {
                // writeImage(image, stream)
                graphics::Image* image = ImageWrapper::Get(L, 1);
                io::Stream* stream = StreamWrapper::Get(L, 2);
                lua_pushboolean(L, graphics::WriteImage(image, stream));
            }
            return 1;
        }

        /**********************************************************
         *                       TIMER MODULE
         **********************************************************/

        //---------------------------------------------------------
        int timer_getTicks(lua_State* L)
        {
            lua_pushnumber(L, timer::GetTicks());
            return 1;
        }

        //---------------------------------------------------------
        int timer_getTime(lua_State* L)
        {
            lua_pushnumber(L, timer::GetTime());
            return 1;
        }

        //---------------------------------------------------------
        int timer_getTimeInfo(lua_State* L)
        {
            u32  rawtime = luaL_checkunsigned(L, 1);
            bool aslocal = lua_toboolean(L, 2);
            luaL_argcheck(L, lua_isnumber(L, 2) || lua_isboolean(L, 2), 2, "expected number or boolean");

            timer::TimeInfo ti = timer::GetTimeInfo(rawtime, aslocal);

            luabridge::LuaRef t = luabridge::newTable(L);

            t["second"]  = ti.second;
            t["minute"]  = ti.minute;
            t["hour"]    = ti.hour;
            t["day"]     = ti.day;
            t["month"]   = ti.month;
            t["year"]    = ti.year;
            t["weekday"] = ti.weekday;
            t["yearday"] = ti.yearday;

            luabridge::push(L, t);

            return 1;
        }

        /**********************************************************
         *                       MOUSE MODULE
         **********************************************************/

        //---------------------------------------------------------
        bool get_mousebutton_constant(int mbutton, std::string& out_mbutton_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (input::MOUSEBUTTON_LEFT,   "left"  )
                (input::MOUSEBUTTON_RIGHT,  "right" )
                (input::MOUSEBUTTON_MIDDLE, "middle")
                (input::MOUSEBUTTON_X1,     "x1"    )
                (input::MOUSEBUTTON_X2,     "x2"    );

            map_type::iterator mapped_value = map.find(mbutton);

            if (mapped_value == map.end()) {
                return false;
            }

            out_mbutton_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_mousebutton_constant(const std::string& mbutton_str, int& out_mbutton)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("left",   input::MOUSEBUTTON_LEFT  )
                ("right",  input::MOUSEBUTTON_RIGHT )
                ("middle", input::MOUSEBUTTON_MIDDLE)
                ("x1",     input::MOUSEBUTTON_X1    )
                ("x2",     input::MOUSEBUTTON_X2    );

            map_type::iterator mapped_value = map.find(mbutton_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_mbutton = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        int mouse_get_x()
        {
            return input::GetMousePosition().x;
        }

        //---------------------------------------------------------
        int mouse_get_y()
        {
            return input::GetMousePosition().y;
        }

        //---------------------------------------------------------
        int mouse_getPosition(lua_State* L)
        {
            Vec2i pos = input::GetMousePosition();
            lua_pushnumber(L, pos.x);
            lua_pushnumber(L, pos.y);
            return 2;
        }

        //---------------------------------------------------------
        int mouse_isDown(lua_State* L)
        {
            int nargs = lua_gettop(L);
            bool result = false;
            for (int i = 1; i <= nargs; i++) {
                const char* mbutton_str = luaL_checkstring(L, i);
                int mbutton;
                if (!get_mousebutton_constant(mbutton_str, mbutton)) {
                    return luaL_argerror(L, i, mbutton_str);
                }
                result |= input::IsMouseButtonPressed(mbutton);
            }
            lua_pushboolean(L, result);
            return 1;
        }

        //---------------------------------------------------------
        int mouse_isAnyDown(lua_State* L)
        {
            lua_pushboolean(L, input::IsAnyMouseButtonPressed());
            return 1;
        }

        /**********************************************************
         *                    KEYBOARD MODULE
         **********************************************************/

        //---------------------------------------------------------
        bool get_key_constant(int key, std::string& out_key_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (input::KEY_BACKSPACE,      "backspace"     )
                (input::KEY_TAB,            "tab"           )
                (input::KEY_CLEAR,          "clear"         )
                (input::KEY_ENTER,          "enter"         )
                (input::KEY_SHIFT,          "shift"         )
                (input::KEY_CONTROL,        "ctrl"          )
                (input::KEY_ALT,            "alt"           )
                (input::KEY_CAPSLOCK,       "caps lock"     )
                (input::KEY_ESCAPE,         "escape"        )
                (input::KEY_SPACE,          "space"         )
                (input::KEY_PAGEUP,         "page up"       )
                (input::KEY_PAGEDOWN,       "page down"     )
                (input::KEY_END,            "end"           )
                (input::KEY_HOME,           "home"          )
                (input::KEY_LEFT,           "left"          )
                (input::KEY_UP,             "up"            )
                (input::KEY_RIGHT,          "right"         )
                (input::KEY_DOWN,           "down"          )
                (input::KEY_PRINTSCREEN,    "print screen"  )
                (input::KEY_INSERT,         "insert"        )
                (input::KEY_DELETE,         "delete"        )
                (input::KEY_0,              "0"             )
                (input::KEY_1,              "1"             )
                (input::KEY_2,              "2"             )
                (input::KEY_3,              "3"             )
                (input::KEY_4,              "4"             )
                (input::KEY_5,              "5"             )
                (input::KEY_6,              "6"             )
                (input::KEY_7,              "7"             )
                (input::KEY_8,              "8"             )
                (input::KEY_9,              "9"             )
                (input::KEY_A,              "a"             )
                (input::KEY_B,              "b"             )
                (input::KEY_C,              "c"             )
                (input::KEY_D,              "d"             )
                (input::KEY_E,              "e"             )
                (input::KEY_F,              "f"             )
                (input::KEY_G,              "g"             )
                (input::KEY_H,              "h"             )
                (input::KEY_I,              "i"             )
                (input::KEY_J,              "j"             )
                (input::KEY_K,              "k"             )
                (input::KEY_L,              "l"             )
                (input::KEY_M,              "m"             )
                (input::KEY_N,              "n"             )
                (input::KEY_O,              "o"             )
                (input::KEY_P,              "p"             )
                (input::KEY_Q,              "q"             )
                (input::KEY_R,              "r"             )
                (input::KEY_S,              "s"             )
                (input::KEY_T,              "t"             )
                (input::KEY_U,              "u"             )
                (input::KEY_V,              "v"             )
                (input::KEY_W,              "w"             )
                (input::KEY_X,              "x"             )
                (input::KEY_Y,              "y"             )
                (input::KEY_Z,              "z"             )
                (input::KEY_NUMPAD0,        "np0"           )
                (input::KEY_NUMPAD1,        "np1"           )
                (input::KEY_NUMPAD2,        "np2"           )
                (input::KEY_NUMPAD3,        "np3"           )
                (input::KEY_NUMPAD4,        "np4"           )
                (input::KEY_NUMPAD5,        "np5"           )
                (input::KEY_NUMPAD6,        "np6"           )
                (input::KEY_NUMPAD7,        "np7"           )
                (input::KEY_NUMPAD8,        "np8"           )
                (input::KEY_NUMPAD9,        "np9"           )
                (input::KEY_MULTIPLY,       "multiply"      )
                (input::KEY_ADD,            "add"           )
                (input::KEY_SUBTRACT,       "subtract"      )
                (input::KEY_DECIMAL,        "decimal"       )
                (input::KEY_DIVIDE,         "divide"        )
                (input::KEY_F1,             "f1"            )
                (input::KEY_F2,             "f2"            )
                (input::KEY_F3,             "f3"            )
                (input::KEY_F4,             "f4"            )
                (input::KEY_F5,             "f5"            )
                (input::KEY_F6,             "f6"            )
                (input::KEY_F7,             "f7"            )
                (input::KEY_F8,             "f8"            )
                (input::KEY_F9,             "f9"            )
                (input::KEY_F10,            "f10"           )
                (input::KEY_F11,            "f11"           )
                (input::KEY_F12,            "f12"           )
                (input::KEY_NUMLOCK,        "num lock"      )
                (input::KEY_SCROLLLOCK,     "scroll lock"   )
                (input::KEY_LSHIFT,         "lshift"        )
                (input::KEY_RSHIFT,         "rshift"        )
                (input::KEY_LCONTROL,       "lctrl"         )
                (input::KEY_RCONTROL,       "rctrl"         )
                (input::KEY_LALT,           "lalt"          )
                (input::KEY_RALT,           "ralt"          );

            map_type::iterator mapped_value = map.find(key);

            if (mapped_value == map.end()) {
                return false;
            }

            out_key_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_key_constant(const std::string& key_str, int& out_key)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("backspace",    input::KEY_BACKSPACE   )
                ("tab",          input::KEY_TAB         )
                ("clear",        input::KEY_CLEAR       )
                ("enter",        input::KEY_ENTER       )
                ("shift",        input::KEY_SHIFT       )
                ("ctrl",         input::KEY_CONTROL     )
                ("alt",          input::KEY_ALT         )
                ("caps lock",    input::KEY_CAPSLOCK    )
                ("escape",       input::KEY_ESCAPE      )
                ("space",        input::KEY_SPACE       )
                ("page up",      input::KEY_PAGEUP      )
                ("page down",    input::KEY_PAGEDOWN    )
                ("end",          input::KEY_END         )
                ("home",         input::KEY_HOME        )
                ("left",         input::KEY_LEFT        )
                ("up",           input::KEY_UP          )
                ("right",        input::KEY_RIGHT       )
                ("down",         input::KEY_DOWN        )
                ("print screen", input::KEY_PRINTSCREEN )
                ("insert",       input::KEY_INSERT      )
                ("delete",       input::KEY_DELETE      )
                ("0",            input::KEY_0           )
                ("1",            input::KEY_1           )
                ("2",            input::KEY_2           )
                ("3",            input::KEY_3           )
                ("4",            input::KEY_4           )
                ("5",            input::KEY_5           )
                ("6",            input::KEY_6           )
                ("7",            input::KEY_7           )
                ("8",            input::KEY_8           )
                ("9",            input::KEY_9           )
                ("a",            input::KEY_A           )
                ("b",            input::KEY_B           )
                ("c",            input::KEY_C           )
                ("d",            input::KEY_D           )
                ("e",            input::KEY_E           )
                ("f",            input::KEY_F           )
                ("g",            input::KEY_G           )
                ("h",            input::KEY_H           )
                ("i",            input::KEY_I           )
                ("j",            input::KEY_J           )
                ("k",            input::KEY_K           )
                ("l",            input::KEY_L           )
                ("m",            input::KEY_M           )
                ("n",            input::KEY_N           )
                ("o",            input::KEY_O           )
                ("p",            input::KEY_P           )
                ("q",            input::KEY_Q           )
                ("r",            input::KEY_R           )
                ("s",            input::KEY_S           )
                ("t",            input::KEY_T           )
                ("u",            input::KEY_U           )
                ("v",            input::KEY_V           )
                ("w",            input::KEY_W           )
                ("x",            input::KEY_X           )
                ("y",            input::KEY_Y           )
                ("z",            input::KEY_Z           )
                ("np0",          input::KEY_NUMPAD0     )
                ("np1",          input::KEY_NUMPAD1     )
                ("np2",          input::KEY_NUMPAD2     )
                ("np3",          input::KEY_NUMPAD3     )
                ("np4",          input::KEY_NUMPAD4     )
                ("np5",          input::KEY_NUMPAD5     )
                ("np6",          input::KEY_NUMPAD6     )
                ("np7",          input::KEY_NUMPAD7     )
                ("np8",          input::KEY_NUMPAD8     )
                ("np9",          input::KEY_NUMPAD9     )
                ("multiply",     input::KEY_MULTIPLY    )
                ("add",          input::KEY_ADD         )
                ("subtract",     input::KEY_SUBTRACT    )
                ("decimal",      input::KEY_DECIMAL     )
                ("divide",       input::KEY_DIVIDE      )
                ("f1",           input::KEY_F1          )
                ("f2",           input::KEY_F2          )
                ("f3",           input::KEY_F3          )
                ("f4",           input::KEY_F4          )
                ("f5",           input::KEY_F5          )
                ("f6",           input::KEY_F6          )
                ("f7",           input::KEY_F7          )
                ("f8",           input::KEY_F8          )
                ("f9",           input::KEY_F9          )
                ("f10",          input::KEY_F10         )
                ("f11",          input::KEY_F11         )
                ("f12",          input::KEY_F12         )
                ("num lock",     input::KEY_NUMLOCK     )
                ("scroll lock",  input::KEY_SCROLLLOCK  )
                ("lshift",       input::KEY_LSHIFT      )
                ("rshift",       input::KEY_RSHIFT      )
                ("lctrl",        input::KEY_LCONTROL    )
                ("rctrl",        input::KEY_RCONTROL    )
                ("lalt",         input::KEY_LALT        )
                ("ralt",         input::KEY_RALT        );

            map_type::iterator mapped_value = map.find(key_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_key = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        int keyboard_isDown(lua_State* L)
        {
            int nargs = lua_gettop(L);
            bool result = false;
            for (int i = 1; i <= nargs; i++) {
                const char* key_str = luaL_checkstring(L, i);
                int key;
                if (!get_key_constant(key_str, key)) {
                    return luaL_argerror(L, i, key_str);
                }
                result |= input::IsKeyPressed(key);
            }
            lua_pushboolean(L, result);
            return 1;
        }

        //---------------------------------------------------------
        int keyboard_isAnyDown(lua_State* L)
        {
            lua_pushboolean(L, input::IsAnyKeyPressed());
            return 1;
        }

        /***********************************************************
         *                       RPG MODULE
         **********************************************************/

        //---------------------------------------------------------
        bool get_scene_constant(int scene, std::string& out_scene_str)
        {
            typedef boost::unordered_map<int, std::string> map_type;

            static map_type map = boost::assign::map_list_of
                (RPG::SCENE_MAP,       "map"      )
                (RPG::SCENE_MENU,      "menu"     )
                (RPG::SCENE_BATTLE,    "battle"   )
                (RPG::SCENE_SHOP,      "shop"     )
                (RPG::SCENE_NAME,      "name"     )
                (RPG::SCENE_FILE,      "file"     )
                (RPG::SCENE_TITLE,     "title"    )
                (RPG::SCENE_GAME_OVER, "game over")
                (RPG::SCENE_DEBUG,     "debug"    );

            map_type::iterator mapped_value = map.find(scene);

            if (mapped_value == map.end()) {
                return false;
            }

            out_scene_str = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        bool get_scene_constant(const std::string& scene_str, int& out_scene)
        {
            typedef boost::unordered_map<std::string, int> map_type;

            static map_type map = boost::assign::map_list_of
                ("map",       RPG::SCENE_MAP      )
                ("menu",      RPG::SCENE_MENU     )
                ("battle",    RPG::SCENE_BATTLE   )
                ("shop",      RPG::SCENE_SHOP     )
                ("name",      RPG::SCENE_NAME     )
                ("file",      RPG::SCENE_FILE     )
                ("title",     RPG::SCENE_TITLE    )
                ("game over", RPG::SCENE_GAME_OVER)
                ("debug",     RPG::SCENE_DEBUG    );

            map_type::iterator mapped_value = map.find(scene_str);

            if (mapped_value == map.end()) {
                return false;
            }

            out_scene = mapped_value->second;
            return true;
        }

        //---------------------------------------------------------
        std::string rpg_get_scene()
        {
            std::string scene_str;
            if (!get_scene_constant(RPG::system->scene, scene_str)) {
                throw "unexpected internal value";
            }
            return scene_str;
        }

        //---------------------------------------------------------
        bool rpg_get_isTestPlay()
        {
            return RPG::isTestPlay;
        }

        //---------------------------------------------------------
        bool rpg_get_isBattleTest()
        {
            return RPG::isBattleTest;
        }

        //---------------------------------------------------------
        int rpg_get_saveCount()
        {
            return RPG::system->saveCount;
        }

        //---------------------------------------------------------
        int rpg_variables_proxy_table_indexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
            if (n > RPG::system->variables.size) {
                RPG::system->variables.resize(n);
            }
            lua_pushnumber(L, RPG::variables[n]);
            return 1;
        }

        //---------------------------------------------------------
        int rpg_variables_proxy_table_newindexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            int v = luaL_checkint(L, 3);
            luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
            if (n > RPG::system->variables.size) {
                RPG::system->variables.resize(n);
            }
            RPG::variables[n] = v;
            return 0;
        }

        //---------------------------------------------------------
        int rpg_variables_proxy_table_lenMetaMethod(lua_State* L)
        {
            lua_pushnumber(L, RPG::system->variables.size);
            return 1;
        }

        //---------------------------------------------------------
        int rpg_variables_proxy_table_resize(lua_State* L)
        {
            int n = luaL_checkint(L, 1);
            luaL_argcheck(L, n >= 0 && n <= RPGSS_SANE_VARIABLE_ARRAY_SIZE_LIMIT, 2, "invalid size");
            RPG::system->variables.resize(n);
            return 0;
        }

        //---------------------------------------------------------
        luabridge::LuaRef rpg_get_variables()
        {
            lua_State* L = Context::Current().interpreter();

            lua_pushstring(L, "__rpg_variables_proxy_table");
            lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

            if (!lua_istable(L, -1)) {
                lua_newtable(L); // proxy table

                lua_pushstring(L, "resize");
                lua_pushcfunction(L, rpg_variables_proxy_table_resize);
                lua_rawset(L, -3); // add resize() to proxy table

                lua_newtable(L); // metatable

                lua_pushstring(L, "__index");
                lua_pushcfunction(L, rpg_variables_proxy_table_indexMetaMethod);
                lua_rawset(L, -3); // add __index() to metatable

                lua_pushstring(L, "__newindex");
                lua_pushcfunction(L, rpg_variables_proxy_table_newindexMetaMethod);
                lua_rawset(L, -3); // add __newindex() to metatable

                lua_pushstring(L, "__len");
                lua_pushcfunction(L, rpg_variables_proxy_table_lenMetaMethod);
                lua_rawset(L, -3); // add __len() to metatable

                lua_setmetatable(L, -2); // set metatable for proxy table

                lua_pushstring(L, "__rpg_variables_proxy_table");
                lua_pushvalue(L, -2); // push proxy table
                lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                assert(lua_istable(L, -1));
            }

            return luabridge::LuaRef::fromStack(L, -1);
        }

        //---------------------------------------------------------
        int rpg_switches_proxy_table_indexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
            if (n > RPG::system->switches.size) {
                RPG::system->switches.resize(n);
            }
            lua_pushboolean(L, RPG::switches[n]);
            return 1;
        }

        //---------------------------------------------------------
        int rpg_switches_proxy_table_newindexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            bool v = lua_toboolean(L, 3);
            luaL_argcheck(L, n >= 1 && n <= RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT, 2, "index out of bounds");
            luaL_argcheck(L, lua_isboolean(L, 3) || lua_isnumber(L, 3), 3, "expected number or boolean");
            if (n > RPG::system->switches.size) {
                RPG::system->switches.resize(n);
            }
            RPG::switches[n] = v;
            return 0;
        }

        //---------------------------------------------------------
        int rpg_switches_proxy_table_lenMetaMethod(lua_State* L)
        {
            lua_pushnumber(L, RPG::system->switches.size);
            return 1;
        }

        //---------------------------------------------------------
        int rpg_switches_proxy_table_resize(lua_State* L)
        {
            int n = luaL_checkint(L, 1);
            luaL_argcheck(L, n >= 0 && n <= RPGSS_SANE_SWITCH_ARRAY_SIZE_LIMIT, 2, "invalid size");
            RPG::system->switches.resize(n);
            return 0;
        }

        //---------------------------------------------------------
        luabridge::LuaRef rpg_get_switches()
        {
            lua_State* L = Context::Current().interpreter();

            lua_pushstring(L, "__rpg_switches_proxy_table");
            lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

            if (!lua_istable(L, -1)) {
                lua_newtable(L); // proxy table

                lua_pushstring(L, "resize");
                lua_pushcfunction(L, rpg_switches_proxy_table_resize);
                lua_rawset(L, -3); // add resize() to proxy table

                lua_newtable(L); // metatable

                lua_pushstring(L, "__index");
                lua_pushcfunction(L, rpg_switches_proxy_table_indexMetaMethod);
                lua_rawset(L, -3); // add __index() to metatable

                lua_pushstring(L, "__newindex");
                lua_pushcfunction(L, rpg_switches_proxy_table_newindexMetaMethod);
                lua_rawset(L, -3); // add __newindex() to metatable

                lua_pushstring(L, "__len");
                lua_pushcfunction(L, rpg_switches_proxy_table_lenMetaMethod);
                lua_rawset(L, -3); // add __len() to metatable

                lua_setmetatable(L, -2); // set metatable for proxy table

                lua_pushstring(L, "__rpg_switches_proxy_table");
                lua_pushvalue(L, -2); // push proxy table
                lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                assert(lua_istable(L, -1));
            }

            return luabridge::LuaRef::fromStack(L, -1);
        }

        //---------------------------------------------------------
        int rpg_actors_proxy_table_indexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            if (n < 1 || n > RPG::actors.count()) {
               return luaL_error(L, "index out of bounds");
            }
            luabridge::push(L, ActorWrapper(RPG::actors[n]));
            return 1;
        }

        //---------------------------------------------------------
        int rpg_actors_proxy_table_newindexMetaMethod(lua_State* L)
        {
           return luaL_error(L, "operation not allowed");
        }

        //---------------------------------------------------------
        int rpg_actors_proxy_table_lenMetaMethod(lua_State* L)
        {
            lua_pushnumber(L, RPG::actors.count());
            return 1;
        }

        //---------------------------------------------------------
        luabridge::LuaRef rpg_get_actors()
        {
            lua_State* L = Context::Current().interpreter();

            lua_pushstring(L, "__rpg_actors_proxy_table");
            lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

            if (!lua_istable(L, -1)) {
                lua_newtable(L); // proxy table

                lua_newtable(L); // metatable

                lua_pushstring(L, "__index");
                lua_pushcfunction(L, rpg_actors_proxy_table_indexMetaMethod);
                lua_rawset(L, -3); // add __index() to metatable

                lua_pushstring(L, "__newindex");
                lua_pushcfunction(L, rpg_actors_proxy_table_newindexMetaMethod);
                lua_rawset(L, -3); // add __newindex() to metatable

                lua_pushstring(L, "__len");
                lua_pushcfunction(L, rpg_actors_proxy_table_lenMetaMethod);
                lua_rawset(L, -3); // add __len() to metatable

                lua_setmetatable(L, -2); // set metatable for proxy table

                lua_pushstring(L, "__rpg_actors_proxy_table");
                lua_pushvalue(L, -2); // push proxy table
                lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                assert(lua_istable(L, -1));
            }

            return luabridge::LuaRef::fromStack(L, -1);
        }

        //---------------------------------------------------------
        int rpg_get_actor_party_size()
        {
            int party_size = 0;
            for (int i = 0; i < 4; i++) {
                if (RPG::Actor::partyMember(i) == 0) {
                    break;
                }
                party_size++;
            }
            return party_size;
        }

        //---------------------------------------------------------
        int rpg_actor_party_proxy_table_indexMetaMethod(lua_State* L)
        {
            int n = luaL_checkint(L, 2);
            if (n < 1 || n > rpg_get_actor_party_size()) {
               return luaL_error(L, "index out of bounds");
            }
            luabridge::push(L, ActorWrapper(RPG::Actor::partyMember(n-1)));
            return 1;
        }

        //---------------------------------------------------------
        int rpg_actor_party_proxy_table_newindexMetaMethod(lua_State* L)
        {
           return luaL_error(L, "operation not allowed");
        }

        //---------------------------------------------------------
        int rpg_actor_party_proxy_table_lenMetaMethod(lua_State* L)
        {
            lua_pushnumber(L, rpg_get_actor_party_size());
            return 1;
        }

        //---------------------------------------------------------
        luabridge::LuaRef rpg_get_actorParty()
        {
            lua_State* L = Context::Current().interpreter();

            lua_pushstring(L, "__rpg_actor_party_proxy_table");
            lua_rawget(L, LUA_REGISTRYINDEX); // get proxy table from registry table

            if (!lua_istable(L, -1)) {
                lua_newtable(L); // proxy table

                lua_newtable(L); // metatable

                lua_pushstring(L, "__index");
                lua_pushcfunction(L, rpg_actor_party_proxy_table_indexMetaMethod);
                lua_rawset(L, -3); // add __index() to metatable

                lua_pushstring(L, "__newindex");
                lua_pushcfunction(L, rpg_actor_party_proxy_table_newindexMetaMethod);
                lua_rawset(L, -3); // add __newindex() to metatable

                lua_pushstring(L, "__len");
                lua_pushcfunction(L, rpg_actor_party_proxy_table_lenMetaMethod);
                lua_rawset(L, -3); // add __len() to metatable

                lua_setmetatable(L, -2); // set metatable for proxy table

                lua_pushstring(L, "__rpg_actor_party_proxy_table");
                lua_pushvalue(L, -2); // push proxy table
                lua_rawset(L, LUA_REGISTRYINDEX); // store proxy table in registry table

                assert(lua_istable(L, -1));
            }

            return luabridge::LuaRef::fromStack(L, -1);
        }

        //---------------------------------------------------------
        int rpg_newByteArray(lua_State* L)
        {
            int nargs = lua_gettop(L);

            if (nargs == 0) {
                // newByteArray()
                luabridge::push(L, ByteArrayWrapper(ByteArray::New()));
            } else {
                if (lua_isnumber(L, 1)) {
                    // newByteArray(size [, value])
                    int size = luaL_checkint(L, 1);
                    int value = luaL_optint(L, 2, 0);
                    if (size < 0) {
                        return luaL_error(L, "invalid size");
                    }
                    luabridge::push(L, ByteArrayWrapper(ByteArray::New(size, value)));
                } else {
                    // newByteArray(string)
                    size_t len;
                    const char* str = lua_tolstring(L, 1, &len);
                    luabridge::push(L, ByteArrayWrapper(ByteArray::New((const u8*)str, len)));
                }
            }

            return 1;
        }

        //---------------------------------------------------------
        bool RegisterRpgModule(lua_State* L)
        {
            luabridge::getGlobalNamespace(L)
                .beginNamespace("rpg")
                    .beginClass<ByteArrayWrapper>("ByteArray")
                        .addProperty("size",                &ByteArrayWrapper::get_size)
                        .addCFunction("__len",              &ByteArrayWrapper::__len)
                        .addCFunction("__add",              &ByteArrayWrapper::__add)
                        .addCFunction("__concat",           &ByteArrayWrapper::__concat)
                        .addCFunction("toString",           &ByteArrayWrapper::toString)
                        .addCFunction("getByte",            &ByteArrayWrapper::getByte)
                        .addCFunction("setByte",            &ByteArrayWrapper::setByte)
                        .addCFunction("resize",             &ByteArrayWrapper::resize)
                        .addCFunction("memset",             &ByteArrayWrapper::memset)
                        .addCFunction("clear",              &ByteArrayWrapper::clear)
                    .endClass()

                    .addProperty("scene",               &rpg_get_scene)
                    .addProperty("isTestPlay",          &rpg_get_isTestPlay)
                    .addProperty("isBattleTest",        &rpg_get_isBattleTest)
                    .addProperty("saveCount",           &rpg_get_saveCount)
                    .addProperty("variables",           &rpg_get_variables)
                    .addProperty("switches",            &rpg_get_switches)
                    .addProperty("actors",              &rpg_get_actors)
                    .addProperty("actorParty",          &rpg_get_actorParty)
                    .addCFunction("newByteArray",       &rpg_newByteArray)

                    .beginNamespace("map")
                        .beginClass<CharacterWrapper>("Character")
                            .addProperty("id",                      &CharacterWrapper::get_id)
                            .addProperty("name",                    &CharacterWrapper::get_name)
                            .addProperty("charsetFilename",         &CharacterWrapper::get_charsetFilename)
                            .addProperty("charsetID",               &CharacterWrapper::get_charsetID)
                            .addProperty("screenX",                 &CharacterWrapper::get_screenX)
                            .addProperty("screenY",                 &CharacterWrapper::get_screenY)
                            .addProperty("enabled",                 &CharacterWrapper::get_enabled,              &CharacterWrapper::set_enabled)
                            .addProperty("x",                       &CharacterWrapper::get_x,                    &CharacterWrapper::set_x)
                            .addProperty("y",                       &CharacterWrapper::get_y,                    &CharacterWrapper::set_y)
                            .addProperty("offsetX",                 &CharacterWrapper::get_offsetX,              &CharacterWrapper::set_offsetX)
                            .addProperty("offsetY",                 &CharacterWrapper::get_offsetY,              &CharacterWrapper::set_offsetY)
                            .addProperty("direction",               &CharacterWrapper::get_direction,            &CharacterWrapper::set_direction)
                            .addProperty("facing",                  &CharacterWrapper::get_facing,               &CharacterWrapper::set_facing)
                            .addProperty("step",                    &CharacterWrapper::get_step,                 &CharacterWrapper::set_step)
                            .addProperty("transparency",            &CharacterWrapper::get_transparency,         &CharacterWrapper::set_transparency)
                            .addProperty("frequency",               &CharacterWrapper::get_frequency,            &CharacterWrapper::set_frequency)
                            .addProperty("layer",                   &CharacterWrapper::get_layer,                &CharacterWrapper::set_layer)
                            .addProperty("forbidEventOverlap",      &CharacterWrapper::get_forbidEventOverlap,   &CharacterWrapper::set_forbidEventOverlap)
                            .addProperty("animationType",           &CharacterWrapper::get_animationType,        &CharacterWrapper::set_animationType)
                            .addProperty("fixedDirection",          &CharacterWrapper::get_fixedDirection,       &CharacterWrapper::set_fixedDirection)
                            .addProperty("speed",                   &CharacterWrapper::get_speed,                &CharacterWrapper::set_speed)
                            .addProperty("moving",                  &CharacterWrapper::get_moving,               &CharacterWrapper::set_moving)
                            .addProperty("phasing",                 &CharacterWrapper::get_phasing,              &CharacterWrapper::set_phasing)
                            .addCFunction("getScreenPosition",      &CharacterWrapper::getScreenPosition)
                            .addCFunction("getPosition",            &CharacterWrapper::getPosition)
                            .addCFunction("setPosition",            &CharacterWrapper::setPosition)
                            .addCFunction("getPositionOffset",      &CharacterWrapper::getPositionOffset)
                            .addCFunction("setPositionOffset",      &CharacterWrapper::setPositionOffset)
                            .addCFunction("setFixedStep",           &CharacterWrapper::setFixedStep)
                            .addCFunction("setNormalStep",          &CharacterWrapper::setNormalStep)
                            .addCFunction("isMovePossible",         &CharacterWrapper::isMovePossible)
                            .addCFunction("stop",                   &CharacterWrapper::stop)
                            .addCFunction("doStep",                 &CharacterWrapper::doStep)
                        .endClass()

                        .deriveClass<EventWrapper, CharacterWrapper>("Event")
                        .endClass()

                        .deriveClass<HeroWrapper, CharacterWrapper>("Hero")
                            .addProperty("vehicle",                 &HeroWrapper::get_vehicle)
                            .addProperty("controlMode",             &HeroWrapper::get_controlMode,              &HeroWrapper::set_controlMode)
                        .endClass()

                        .addProperty("hero",                    &map_get_hero)
                        .addProperty("events",                  &map_get_events)
                        .addProperty("id",                      &map_get_id)
                        .addProperty("width",                   &map_get_width)
                        .addProperty("height",                  &map_get_height)
                        .addProperty("cameraX",                 &map_get_cameraX)
                        .addProperty("cameraY",                 &map_get_cameraY)
                        .addCFunction("findEventByID",          &map_findEventByID)
                        .addCFunction("findEventByName",        &map_findEventByName)
                        .addCFunction("getCameraPosition",      &map_getCameraPosition)
                        .addCFunction("setCameraPosition",      &map_setCameraPosition)
                        .addCFunction("moveCamera",             &map_moveCamera)
                        .addCFunction("getLowerLayerTileID",    &map_getLowerLayerTileID)
                        .addCFunction("getUpperLayerTileID",    &map_getUpperLayerTileID)
                        .addCFunction("getTerrainID",           &map_getTerrainID)
                    .endNamespace()

                    .beginNamespace("battle")
                        .beginClass<BattlerWrapper>("Battler")
                            .addProperty("databaseID",              &BattlerWrapper::get_databaseID)
                            .addProperty("name",                    &BattlerWrapper::get_name)
                            .addProperty("isMonster",               &BattlerWrapper::get_isMonster)
                            .addProperty("notHidden",               &BattlerWrapper::get_notHidden)
                            .addProperty("mirrored",                &BattlerWrapper::get_mirrored)
                            .addProperty("hp",                      &BattlerWrapper::get_hp,                    &BattlerWrapper::set_hp)
                            .addProperty("mp",                      &BattlerWrapper::get_mp,                    &BattlerWrapper::set_mp)
                            .addProperty("maxHP",                   &BattlerWrapper::get_maxHP)
                            .addProperty("maxMP",                   &BattlerWrapper::get_maxMP)
                            .addProperty("attack",                  &BattlerWrapper::get_attack)
                            .addProperty("defense",                 &BattlerWrapper::get_defense)
                            .addProperty("intelligence",            &BattlerWrapper::get_intelligence)
                            .addProperty("agility",                 &BattlerWrapper::get_agility)
                            .addProperty("atbValue",                &BattlerWrapper::get_atbValue)
                            .addProperty("turnsTaken",              &BattlerWrapper::get_turnsTaken)
                            .addProperty("screenX",                 &BattlerWrapper::get_screenX)
                            .addProperty("screenY",                 &BattlerWrapper::get_screenY)
                            .addCFunction("getScreenPosition",      &BattlerWrapper::getScreenPosition)
                        .endClass()

                        .deriveClass<MonsterWrapper, BattlerWrapper>("Monster")
                            .addProperty("monsterPartyID", &MonsterWrapper::get_monsterPartyID)
                        .endClass()

                        .deriveClass<ActorWrapper, BattlerWrapper>("Actor")
                            .addProperty("degree",                  &ActorWrapper::get_degree)
                            .addProperty("charsetFlename",          &ActorWrapper::get_charsetFilename)
                            .addProperty("charsetID",               &ActorWrapper::get_charsetID)
                            .addProperty("facesetFilename",         &ActorWrapper::get_facesetFilename)
                            .addProperty("facesetID",               &ActorWrapper::get_facesetID)
                            .addProperty("level",                   &ActorWrapper::get_level,                   &ActorWrapper::set_level)
                            .addProperty("exp",                     &ActorWrapper::get_exp,                     &ActorWrapper::set_exp)
                            .addProperty("weaponID",                &ActorWrapper::get_weaponID,                &ActorWrapper::set_weaponID)
                            .addProperty("shieldID",                &ActorWrapper::get_shieldID,                &ActorWrapper::set_shieldID)
                            .addProperty("armorID",                 &ActorWrapper::get_armorID,                 &ActorWrapper::set_armorID)
                            .addProperty("helmetID",                &ActorWrapper::get_helmetID,                &ActorWrapper::set_helmetID)
                            .addProperty("accessoryID",             &ActorWrapper::get_accessoryID,             &ActorWrapper::set_accessoryID)
                            .addProperty("twoWeapons",              &ActorWrapper::get_twoWeapons,              &ActorWrapper::set_twoWeapons)
                            .addProperty("lockEquipment",           &ActorWrapper::get_lockEquipment,           &ActorWrapper::set_lockEquipment)
                            .addProperty("autoBattle",              &ActorWrapper::get_autoBattle,              &ActorWrapper::set_autoBattle)
                            .addProperty("mightyGuard",             &ActorWrapper::get_mightyGuard,             &ActorWrapper::set_mightyGuard)
                        .endClass()

                        .addProperty("monsterParty",            &battle_get_monsterParty)
                    .endNamespace()

                    .beginNamespace("io")
                        .beginClass<StreamWrapper>("Stream")
                            .addProperty("eof",                 &StreamWrapper::get_eof)
                            .addProperty("error",               &StreamWrapper::get_error)
                            .addProperty("good",                &StreamWrapper::get_good)
                            .addCFunction("clearError",         &StreamWrapper::clearError)
                            .addCFunction("seek",               &StreamWrapper::seek)
                            .addCFunction("tell",               &StreamWrapper::tell)
                            .addCFunction("read",               &StreamWrapper::read)
                            .addCFunction("write",              &StreamWrapper::write)
                            .addCFunction("flush",              &StreamWrapper::flush)
                        .endClass()

                        .deriveClass<FileStreamWrapper, StreamWrapper>("FileStream")
                            .addProperty("isOpen",              &FileStreamWrapper::get_isOpen)
                            .addProperty("size",                &FileStreamWrapper::get_size)
                            .addCFunction("__len",              &FileStreamWrapper::__len)
                            .addCFunction("close",              &FileStreamWrapper::close)
                        .endClass()

                        .deriveClass<MemoryStreamWrapper, StreamWrapper>("MemoryStream")
                            .addProperty("capacity",            &MemoryStreamWrapper::get_capacity)
                            .addProperty("size",                &MemoryStreamWrapper::get_size)
                            .addCFunction("__len",              &MemoryStreamWrapper::__len)
                            .addCFunction("copyBuffer",         &MemoryStreamWrapper::copyBuffer)
                            .addCFunction("reserve",            &MemoryStreamWrapper::reserve)
                            .addCFunction("clear",              &MemoryStreamWrapper::clear)
                        .endClass()

                        .beginClass<BinaryReaderWrapper>("BinaryReader")
                            .addProperty("stream",              &BinaryReaderWrapper::get_stream)
                            .addCFunction("skip",               &BinaryReaderWrapper::skip)
                            .addCFunction("readByte",           &BinaryReaderWrapper::readByte)
                            .addCFunction("readBytes",          &BinaryReaderWrapper::readBytes)
                            .addCFunction("readString",         &BinaryReaderWrapper::readString)
                            .addCFunction("readBool",           &BinaryReaderWrapper::readBool)
                            .addCFunction("readInt8",           &BinaryReaderWrapper::readInt8)
                            .addCFunction("readInt16",          &BinaryReaderWrapper::readInt16)
                            .addCFunction("readInt16BE",        &BinaryReaderWrapper::readInt16BE)
                            .addCFunction("readInt32",          &BinaryReaderWrapper::readInt32)
                            .addCFunction("readInt32BE",        &BinaryReaderWrapper::readInt32BE)
                            .addCFunction("readUint8",          &BinaryReaderWrapper::readUint8)
                            .addCFunction("readUint16",         &BinaryReaderWrapper::readUint16)
                            .addCFunction("readUint16BE",       &BinaryReaderWrapper::readUint16BE)
                            .addCFunction("readUint32",         &BinaryReaderWrapper::readUint32)
                            .addCFunction("readUint32BE",       &BinaryReaderWrapper::readUint32BE)
                            .addCFunction("readFloat",          &BinaryReaderWrapper::readFloat)
                            .addCFunction("readFloatBE",        &BinaryReaderWrapper::readFloatBE)
                            .addCFunction("readDouble",         &BinaryReaderWrapper::readDouble)
                            .addCFunction("readDoubleBE",       &BinaryReaderWrapper::readDoubleBE)
                        .endClass()

                        .beginClass<BinaryWriterWrapper>("BinaryWriter")
                            .addProperty("stream",              &BinaryWriterWrapper::get_stream)
                            .addCFunction("writeByte",          &BinaryWriterWrapper::writeByte)
                            .addCFunction("writeBytes",         &BinaryWriterWrapper::writeBytes)
                            .addCFunction("writeString",        &BinaryWriterWrapper::writeString)
                            .addCFunction("writeBool",          &BinaryWriterWrapper::writeBool)
                            .addCFunction("writeInt8",          &BinaryWriterWrapper::writeInt8)
                            .addCFunction("writeInt16",         &BinaryWriterWrapper::writeInt16)
                            .addCFunction("writeInt16BE",       &BinaryWriterWrapper::writeInt16BE)
                            .addCFunction("writeInt32",         &BinaryWriterWrapper::writeInt32)
                            .addCFunction("writeInt32BE",       &BinaryWriterWrapper::writeInt32BE)
                            .addCFunction("writeUint8",         &BinaryWriterWrapper::writeUint8)
                            .addCFunction("writeUint16",        &BinaryWriterWrapper::writeUint16)
                            .addCFunction("writeUint16BE",      &BinaryWriterWrapper::writeUint16BE)
                            .addCFunction("writeUint32",        &BinaryWriterWrapper::writeUint32)
                            .addCFunction("writeUint32BE",      &BinaryWriterWrapper::writeUint32BE)
                            .addCFunction("writeFloat",         &BinaryWriterWrapper::writeFloat)
                            .addCFunction("writeFloatBE",       &BinaryWriterWrapper::writeFloatBE)
                            .addCFunction("writeDouble",        &BinaryWriterWrapper::writeDouble)
                            .addCFunction("writeDoubleBE",      &BinaryWriterWrapper::writeDoubleBE)
                        .endClass()

                        .addCFunction("newMemoryStream",        &io_newMemoryStream)
                        .addCFunction("newBinaryReader",        &io_newBinaryReader)
                        .addCFunction("newBinaryWriter",        &io_newBinaryWriter)
                        .addCFunction("openFile",               &io_openFile)
                        .addCFunction("makeDirectory",          &io_makeDirectory)
                        .addCFunction("remove",                 &io_remove)
                        .addCFunction("enumerate",              &io_enumerate)
                        .addCFunction("exists",                 &io_exists)
                        .addCFunction("lastWriteTime",          &io_lastWriteTime)
                        .addCFunction("isDirectory",            &io_isDirectory)
                        .addCFunction("isFile",                 &io_isFile)
                        .addCFunction("mount",                  &io_mount)
                        .addCFunction("unmount",                &io_unmount)
                        .addCFunction("compress",               &io_compress)
                        .addCFunction("decompress",             &io_decompress)
                    .endNamespace()

                    .beginNamespace("timer")
                        .addCFunction("getTicks",               &timer_getTicks)
                        .addCFunction("getTime",                &timer_getTime)
                        .addCFunction("getTimeInfo",            &timer_getTimeInfo)
                    .endNamespace()

                    .beginNamespace("mouse")
                        .addProperty("x",                       &mouse_get_x)
                        .addProperty("y",                       &mouse_get_y)
                        .addCFunction("getPosition",            &mouse_getPosition)
                        .addCFunction("isDown",                 &mouse_isDown)
                        .addCFunction("isAnyDown",              &mouse_isAnyDown)
                    .endNamespace()

                    .beginNamespace("keyboard")
                        .addCFunction("isDown",                 &keyboard_isDown)
                        .addCFunction("isAnyDown",              &keyboard_isAnyDown)
                    .endNamespace()

                    .beginNamespace("input")
                        .addCFunction("isDown",                 &input_isDown)
                        .addCFunction("getKeyCodes",            &input_getKeyCodes)
                        .addCFunction("setKeyCodes",            &input_setKeyCodes)
                    .endNamespace()

                    .beginNamespace("audio")
                        .beginClass<SoundWrapper>("Sound")
                            .addProperty("isPlaying",           &SoundWrapper::get_isPlaying)
                            .addProperty("isSeekable",          &SoundWrapper::get_isSeekable)
                            .addProperty("length",              &SoundWrapper::get_length)
                            .addProperty("repeat",              &SoundWrapper::get_repeat,              &SoundWrapper::set_repeat)
                            .addProperty("volume",              &SoundWrapper::get_volume,              &SoundWrapper::set_volume)
                            .addProperty("position",            &SoundWrapper::get_position,            &SoundWrapper::set_position)
                            .addCFunction("__len",              &SoundWrapper::__len)
                            .addCFunction("play",               &SoundWrapper::play)
                            .addCFunction("stop",               &SoundWrapper::stop)
                            .addCFunction("reset",              &SoundWrapper::reset)
                        .endClass()

                        .beginClass<SoundEffectWrapper>("SoundEffect")
                            .addProperty("volume",              &SoundEffectWrapper::get_volume,        &SoundEffectWrapper::set_volume)
                            .addCFunction("play",               &SoundEffectWrapper::play)
                            .addCFunction("stop",               &SoundEffectWrapper::stop)
                        .endClass()

                        .addCFunction("openSound",              &audio_openSound)
                        .addCFunction("openSoundEffect",        &audio_openSoundEffect)
                        .addCFunction("playSystemBGM",          &audio_playSystemBGM)
                        .addCFunction("playBGM",                &audio_playBGM)
                        .addCFunction("fadeBGM",                &audio_fadeBGM)
                        .addCFunction("stopBGM",                &audio_stopBGM)
                        .addCFunction("playSystemSE",           &audio_playSystemSE)
                        .addCFunction("playSE",                 &audio_playSE)
                        .addCFunction("stopSE",                 &audio_stopSE)
                    .endNamespace()

                    .beginNamespace("graphics")
                        .beginClass<ImageWrapper>("Image")
                            .addProperty("width",               &ImageWrapper::get_width)
                            .addProperty("height",              &ImageWrapper::get_height)
                            .addCFunction("__len",              &ImageWrapper::__len)
                            .addCFunction("getDimensions",      &ImageWrapper::getDimensions)
                            .addCFunction("getBlendMode",       &ImageWrapper::getBlendMode)
                            .addCFunction("setBlendMode",       &ImageWrapper::setBlendMode)
                            .addCFunction("getClipRect",        &ImageWrapper::getClipRect)
                            .addCFunction("setClipRect",        &ImageWrapper::setClipRect)
                            .addCFunction("copyPixels",         &ImageWrapper::copyPixels)
                            .addCFunction("copyRect",           &ImageWrapper::copyRect)
                            .addCFunction("resize",             &ImageWrapper::resize)
                            .addCFunction("setAlpha",           &ImageWrapper::setAlpha)
                            .addCFunction("clear",              &ImageWrapper::clear)
                            .addCFunction("grey",               &ImageWrapper::grey)
                            .addCFunction("flip",               &ImageWrapper::flip)
                            .addCFunction("rotate",             &ImageWrapper::rotate)
                            .addCFunction("getPixel",           &ImageWrapper::getPixel)
                            .addCFunction("setPixel",           &ImageWrapper::setPixel)
                            .addCFunction("drawPoint",          &ImageWrapper::drawPoint)
                            .addCFunction("drawLine",           &ImageWrapper::drawLine)
                            .addCFunction("drawRectangle",      &ImageWrapper::drawRectangle)
                            .addCFunction("drawCircle",         &ImageWrapper::drawCircle)
                            .addCFunction("drawTriangle",       &ImageWrapper::drawTriangle)
                            .addCFunction("draw",               &ImageWrapper::draw)
                            .addCFunction("drawq",              &ImageWrapper::drawq)
                        .endClass()

                        .addProperty("width",                   &graphics_get_width)
                        .addProperty("height",                  &graphics_get_height)
                        .addProperty("brightness",              &graphics_get_brightness,           &graphics_set_brightness)
                        .addCFunction("getDimensions",          &graphics_getDimensions)
                        .addCFunction("getBlendMode",           &graphics_getBlendMode)
                        .addCFunction("setBlendMode",           &graphics_setBlendMode)
                        .addCFunction("getClipRect",            &graphics_getClipRect)
                        .addCFunction("setClipRect",            &graphics_setClipRect)
                        .addCFunction("packColor",              &graphics_packColor)
                        .addCFunction("unpackColor",            &graphics_unpackColor)
                        .addCFunction("copyRect",               &graphics_copyRect)
                        .addCFunction("clear",                  &graphics_clear)
                        .addCFunction("grey",                   &graphics_grey)
                        .addCFunction("getPixel",               &graphics_getPixel)
                        .addCFunction("setPixel",               &graphics_setPixel)
                        .addCFunction("drawPoint",              &graphics_drawPoint)
                        .addCFunction("drawLine",               &graphics_drawLine)
                        .addCFunction("drawRectangle",          &graphics_drawRectangle)
                        .addCFunction("drawCircle",             &graphics_drawCircle)
                        .addCFunction("drawTriangle",           &graphics_drawTriangle)
                        .addCFunction("draw",                   &graphics_draw)
                        .addCFunction("drawq",                  &graphics_drawq)
                        .addCFunction("newImage",               &graphics_newImage)
                        .addCFunction("readImage",              &graphics_readImage)
                        .addCFunction("writeImage",             &graphics_writeImage)
                    .endNamespace()
                .endNamespace();

            return true;
        }

    } // namespace script
} // namespace rpgss
