//
// Created by dusan-maric on 8/4/26.
//

#ifndef MATF_RG_PROJECT_WORLDSETTINGS_HPP
#define MATF_RG_PROJECT_WORLDSETTINGS_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct PointLightConfig {
    glm::vec3 position;
    glm::vec3 color;
};

struct WorldConfig {
    std::string name;
    std::string carModelName;
    double carModelScale;
    std::string sideObjectsModelName;
    double sideObjectsModelScale;
    std::vector<glm::vec3> sideObjectPositions;

    glm::vec3 spotLightColor;
    glm::vec3 spotLightPosition;
    std::vector<PointLightConfig> pointLights;
};

class WorldSettings {
public:
    WorldSettings();

    void switch_world();

    const WorldConfig &active_config() const { return m_worlds[m_active_world_index]; }

private:
    std::vector<WorldConfig> m_worlds;
    size_t m_active_world_index = 0;

    void setup_worlds();
};

#endif//MATF_RG_PROJECT_WORLDSETTINGS_HPP