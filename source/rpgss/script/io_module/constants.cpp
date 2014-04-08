#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#include "../../io/File.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace io_module {

            //---------------------------------------------------------
            bool GetOpenModeConstant(const std::string& open_mode_str, int& out_open_mode)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("in",     io::File::In    )
                    ("out",    io::File::Out   )
                    ("append", io::File::Append);

                map_type::iterator mapped_value = map.find(open_mode_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_open_mode = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetSeekModeConstant(const std::string& seek_mode_str, int& out_seek_mode)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("begin",   io::File::Begin  )
                    ("current", io::File::Current)
                    ("end",     io::File::End    );

                map_type::iterator mapped_value = map.find(seek_mode_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_seek_mode = mapped_value->second;
                return true;
            }

        } // namespace io_module
    } // namespace script
} // namespace rpgss
