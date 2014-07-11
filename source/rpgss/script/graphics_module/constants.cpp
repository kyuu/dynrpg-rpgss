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

#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#include "../../graphics/Image.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            //---------------------------------------------------------
            bool GetBlendModeConstant(int blend_mode, std::string& out_blend_mode_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (graphics::BlendMode::Set,      "set")
                    (graphics::BlendMode::Mix,      "mix")
                    (graphics::BlendMode::Add,      "add")
                    (graphics::BlendMode::Subtract, "sub")
                    (graphics::BlendMode::Multiply, "mul");

                map_type::iterator mapped_value = map.find(blend_mode);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_blend_mode_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetBlendModeConstant(const std::string& blend_mode_str, int& out_blend_mode)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("set", graphics::BlendMode::Set     )
                    ("mix", graphics::BlendMode::Mix     )
                    ("add", graphics::BlendMode::Add     )
                    ("sub", graphics::BlendMode::Subtract)
                    ("mul", graphics::BlendMode::Multiply);

                map_type::iterator mapped_value = map.find(blend_mode_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_blend_mode = mapped_value->second;
                return true;
            }

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss
