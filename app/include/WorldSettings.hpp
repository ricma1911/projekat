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
    std::string car_model_name;
    double car_model_scale;
    std::string side_objects_model_name;
    double side_objects_model_scale;
    std::vector<glm::vec3> side_object_positions;

    glm::vec3 spot_light_color;
    glm::vec3 spot_light_position;
    std::vector<PointLightConfig> point_lights;
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