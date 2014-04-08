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
            bool GetBlendModeConstant(const std::string& blend_mode_str, int& out_blend_mode)
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

        } // namespace graphics_module
    } // namespace script
} // namespace rpgss
