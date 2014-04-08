#ifndef RPGSS_SCRIPT_GAME_MODULE_CONSTANTS_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_CONSTANTS_HPP_INCLUDED

#include <string>


namespace rpgss {
    namespace script {
        namespace game_module {

            bool GetDirectionConstant(int direction, std::string& out_direction_str);
            bool GetDirectionConstant(const std::string& direction_str, int& out_direction);
            bool GetFacingConstant(int facing, std::string& out_facing_str);
            bool GetFacingConstant(const std::string& facing_str, int& out_facing);
            bool GetLayerConstant(int layer, std::string& out_layer_str);
            bool GetLayerConstant(const std::string& layer_str, int& out_layer);
            bool GetAnimationTypeConstant(int animationtype, std::string& out_animationtype_str);
            bool GetAnimationTypeConstant(const std::string& animationtype_str, int& out_animationtype);
            bool GetControlModeConstant(int controlmode, std::string& out_controlmode_str);
            bool GetControlModeConstant(const std::string& controlmode_str, int& out_controlmode);
            bool GetVehicleConstant(int vehicle, std::string& out_vehicle_str);
            bool GetVehicleConstant(const std::string& vehicle_str, int& out_vehicle);
            bool GetRpgKeyConstant(int rpg_key, std::string& out_rpg_key_str);
            bool GetRpgKeyConstant(const std::string& rpg_key_str, int& out_rpg_key);
            bool GetSceneConstant(int scene, std::string& out_scene_str);
            bool GetSceneConstant(const std::string& scene_str, int& out_scene);

        } // namespace game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_CONSTANTS_HPP_INCLUDED
