#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#include "../../input/input.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace mouse_module {

            //---------------------------------------------------------
            bool GetMouseButtonConstant(int mbutton, std::string& out_mbutton_str)
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
            bool GetMouseButtonConstant(const std::string& mbutton_str, int& out_mbutton)
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

        } // mouse_module
    } // namespace script
} // namespace rpgss
