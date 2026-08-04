//
// Created by dusan-maric on 8/4/26.
//

#ifndef MATF_RG_PROJECT_WORLDSETTINGS_HPP
#define MATF_RG_PROJECT_WORLDSETTINGS_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct WorldConfig {
    std::string name;
    std::string carModelName;
    std::string sideObjectsModelName;
    std::vector<glm::vec3> sideObjectPositions;
};

class WorldSettings {
public:
    WorldSettings();

    void switch_world();

    const WorldConfig& active_config() const { return m_worlds[m_activeWorldIndex]; }

private:
    std::vector<WorldConfig> m_worlds;
    size_t m_activeWorldIndex = 0;

    void setup_worlds();
};

#endif//MATF_RG_PROJECT_WORLDSETTINGS_HPP