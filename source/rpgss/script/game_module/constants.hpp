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
            bool GetMenuSceneConstant(int menuscene, std::string& out_menuscene_str);
            bool GetMenuSceneConstant(const std::string& menuscene_str, int& out_menuscene);
            bool GetBattleLayoutConstant(int battlelayout, std::string& out_battlelayout_str);
            bool GetBattleLayoutConstant(const std::string& battlelayout_str, int& out_battlelayout);

        } // namespace game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_CONSTANTS_HPP_INCLUDED
