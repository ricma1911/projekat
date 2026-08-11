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
    viceCity.carModelName = "ferrari";
    viceCity.carModelScale = 0.3;
    viceCity.sideObjectsModelName = "palm";
    viceCity.sideObjectsModelScale = 1.0;
    viceCity.sideObjectPositions = {
            glm::vec3(-2.0f, -1.6f, -17.0f),
            glm::vec3(2.0f, -1.6f, -17.0f),
            glm::vec3(-2.0f, -1.6f, -13.0f),
            glm::vec3(2.0f, -1.6f, -13.0f)};

    //glm::vec3(0.0f, -2.5f, -16.5f)
    //glm::vec3(-2.5f, -1.5f, -15.0f),
    //glm::vec3(2.5f, -1.5f, -15.0f)
    viceCity.spotLightColor = glm::vec3(0.0f, 0.94f, 1.0f);
    viceCity.spotLightPosition = glm::vec3(0.0f, -2.5f, -16.5f);
    viceCity.pointLight0Color = glm::vec3(1.0f, 0.5f, 0.0f);
    viceCity.pointLight0Position = glm::vec3(-2.5f, -1.5f, -15.0f);
    viceCity.pointLight1Color = glm::vec3(1.0f, 0.0f, 0.8f);
    viceCity.pointLight1Position = glm::vec3(2.5f, -1.5f, -15.0f);

    WorldConfig losSantos;
    losSantos.name = "Los Santos";
    losSantos.carModelName = "lamborghini_aventador";
    losSantos.carModelScale = 0.003;
    losSantos.sideObjectsModelName = "garbage_container";
    losSantos.sideObjectsModelScale = 0.5;
    losSantos.sideObjectPositions = {
            glm::vec3(-2.0f, -2.0f, -17.0f),
            glm::vec3(2.0f, -2.0f, -17.0f),
            glm::vec3(-2.0f, -2.0f, -13.0f),
            glm::vec3(2.0f, -2.0f, -13.0f)};

    losSantos.spotLightColor = glm::vec3(0.5f, 0.0f, 0.9f);
    losSantos.spotLightPosition = glm::vec3(0.0f, -2.5f, -16.5f);
    losSantos.pointLight0Color = glm::vec3(1.0f, 0.1f, 0.0f);
    losSantos.pointLight0Position = glm::vec3(-2.5f, -1.5f, -15.0f);
    losSantos.pointLight1Color = glm::vec3(0.0f, 1.0f, 0.2f);
    losSantos.pointLight1Position = glm::vec3(2.5f, -1.5f, -15.0f);


    m_worlds.push_back(viceCity);
    m_worlds.push_back(losSantos);
}

void WorldSettings::switch_world() {
    m_active_world_index = (m_active_world_index + 1) % m_worlds.size();
}