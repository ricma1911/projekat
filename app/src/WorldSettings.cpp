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
    viceCity.sideObjectsModelName = "palm";
    viceCity.sideObjectPositions = {
        glm::vec3(-2.0f, -1.6f, -17.0f),
        glm::vec3( 2.0f, -1.6f, -17.0f),
        glm::vec3(-2.0f, -1.6f, -13.0f),
        glm::vec3( 2.0f, -1.6f, -13.0f)
    };

    WorldConfig losSantos;
    losSantos.name = "Los Santos";
    losSantos.carModelName = "lamborghini_aventador";
    losSantos.sideObjectsModelName = "palm";
    losSantos.sideObjectPositions = {
        glm::vec3(-2.0f, -1.6f, -17.0f),
        glm::vec3( 2.0f, -1.6f, -17.0f),
        glm::vec3(-2.0f, -1.6f, -13.0f),
        glm::vec3( 2.0f, -1.6f, -13.0f)
    };

    m_worlds.push_back(viceCity);
    m_worlds.push_back(losSantos);
}

void WorldSettings::switch_world() {
    m_activeWorldIndex = (m_activeWorldIndex + 1) % m_worlds.size();
}