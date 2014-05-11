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

#include "Decompressor.hpp"

#define ZSTREAM_BUFFER_SIZE 4096


namespace rpgss {
    namespace util {

        //-----------------------------------------------------------------
        Decompressor::Decompressor()
            : _good(true)
        {
            _buffer = core::ByteArray::New(ZSTREAM_BUFFER_SIZE);

            _stream.zalloc   = Z_NULL;
            _stream.zfree    = Z_NULL;
            _stream.opaque   = Z_NULL;
            _stream.next_in  = Z_NULL;
            _stream.avail_in = 0;

            inflateInit(&_stream);
        }

        //-----------------------------------------------------------------
        Decompressor::~Decompressor()
        {
            inflateEnd(&_stream);
        }

        //-----------------------------------------------------------------
        bool
        Decompressor::decompress(const u8* buffer, int size, io::File* ostream)
        {
            if (_good) {
                // initialize input parameters
                _stream.next_in  = (Bytef*)buffer;
                _stream.avail_in = size;

                // intialize output parameters
                _stream.next_out  = _buffer->getBuffer();
                _stream.avail_out = _buffer->getSize();

                while (true) {
                    int ret = inflate(&_stream, Z_NO_FLUSH);

                    switch (ret) {
                    case Z_NEED_DICT:
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        _good = false;
                        return false;
                    }

                    if (_stream.avail_in == 0) { // all input has been consumed
                        if ((int)_stream.avail_out < _buffer->getSize()) { // we still have some data in the output buffer
                            int data_size = _buffer->getSize() - (int)_stream.avail_out;
                            if (ostream->write(_buffer->getBuffer(), data_size) != data_size) {
                                return false;
                            }
                        }

                        // we are done
                        return true;

                    } else if (_stream.avail_out == 0) { // output buffer is full
                        int data_size = _buffer->getSize();
                        if (ostream->write(_buffer->getBuffer(), data_size) != data_size) {
                            return false;
                        }

                        // update output parameters and resume input processing
                        _stream.next_out  = _buffer->getBuffer();
                        _stream.avail_out = _buffer->getSize();
                    }
                }
            }

            return false;
        }

        //-----------------------------------------------------------------
        bool
        Decompressor::finish(io::File* ostream)
        {
            if (_good) {
                _good = false;

                // initialize input parameters
                _stream.next_in  = Z_NULL;
                _stream.avail_in = 0;

                // intialize output parameters
                _stream.next_out  = _buffer->getBuffer();
                _stream.avail_out = _buffer->getSize();

                while (true) {
                    int ret = inflate(&_stream, Z_FINISH);

                    switch (ret) {
                    case Z_NEED_DICT:
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        return false;
                    }

                    if (_stream.avail_out == 0) { // output buffer is full
                        int data_size = _buffer->getSize();
                        if (ostream->write(_buffer->getBuffer(), data_size) != data_size) {
                            return false;
                        }

                        // update output parameters and resume input processing
                        _stream.next_out  = _buffer->getBuffer();
                        _stream.avail_out = _buffer->getSize();

                    } else { // all output has been produced
                        if ((int)_stream.avail_out < _buffer->getSize()) { // we still have some data in the output buffer
                            int data_size = _buffer->getSize() - (int)_stream.avail_out;
                            if (ostream->write(_buffer->getBuffer(), data_size) != data_size) {
                                return false;
                            }
                        }

                        // we are done
                        return true;
                    }
                }
            }

            return false;
        }

    } // namespace util
} // namespace rpgss
