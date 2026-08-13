//
// Created by dusan-maric on 8/4/26.
//

#include "WorldSettings.hpp"

WorldSettings::WorldSettings() {
    setup_worlds();
}

void WorldSettings::setup_worlds() {
    WorldConfig viceCity;
    viceCity.name = "Vice City";
    viceCity.car_model_name = "ferrari";
    viceCity.car_model_scale = 0.3;
    viceCity.side_objects_model_name = "palm";
    viceCity.side_objects_model_scale = 1.0;
    viceCity.side_object_positions = {
            glm::vec3(-2.0f, -1.6f, -17.0f),
            glm::vec3(2.0f, -1.6f, -17.0f),
            glm::vec3(-2.0f, -1.6f, -13.0f),
            glm::vec3(2.0f, -1.6f, -13.0f)};

    //glm::vec3(0.0f, -2.5f, -16.5f)
    //glm::vec3(-2.5f, -1.5f, -15.0f),
    //glm::vec3(2.5f, -1.5f, -15.0f)
    viceCity.spot_light_color = glm::vec3(0.0f, 0.94f, 1.0f);
    viceCity.spot_light_position = glm::vec3(0.0f, -2.5f, -16.5f);
    viceCity.point_lights = {
            {glm::vec3(-2.5f, -1.5f, -15.0f), glm::vec3(1.0f, 0.5f, 0.0f)},
            {glm::vec3(2.5f, -1.5f, -15.0f), glm::vec3(1.0f, 0.0f, 0.8f)}};

    WorldConfig losSantos;
    losSantos.name = "Los Santos";
    losSantos.car_model_name = "lamborghini_aventador";
    losSantos.car_model_scale = 0.003;
    losSantos.side_objects_model_name = "garbage_container";
    losSantos.side_objects_model_scale = 0.5;
    losSantos.side_object_positions = {
            glm::vec3(-2.0f, -2.0f, -17.0f),
            glm::vec3(2.0f, -2.0f, -17.0f),
            glm::vec3(-2.0f, -2.0f, -13.0f),
            glm::vec3(2.0f, -2.0f, -13.0f)};

    losSantos.spot_light_color = glm::vec3(0.5f, 0.0f, 0.9f);
    losSantos.spot_light_position = glm::vec3(0.0f, -2.5f, -16.5f);
    losSantos.point_lights = {
            {glm::vec3(-2.5f, -1.5f, -15.0f), glm::vec3(1.0f, 0.1f, 0.0f)},
            {glm::vec3(2.5f, -1.5f, -15.0f), glm::vec3(0.0f, 1.0f, 0.2f)}};


    m_worlds.push_back(viceCity);
    m_worlds.push_back(losSantos);
}

void WorldSettings::switch_world() {
    m_active_world_index = (m_active_world_index + 1) % m_worlds.size();
}