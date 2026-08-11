#ifndef ENGINE_GRAPHICS_POINT_SHADOWS_HPP
#define ENGINE_GRAPHICS_POINT_SHADOWS_HPP

#include <cstdint>
#include <engine/resources/Shader.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace engine::graphics {

class PointShadows {
public:
    PointShadows() = default;
    ~PointShadows();

    void init(uint32_t shadow_width = 1024, uint32_t shadow_height = 1024);

    void bind();
    void unbind(uint32_t current_window_width, uint32_t current_window_height);

    std::vector<glm::mat4> calculate_light_space_matrices(const glm::vec3 &light_pos, float near_plane, float far_plane);

    void bind_depth_map(uint32_t texture_unit = 0) const;
    void bind_face(uint32_t face_index);

    [[nodiscard]] uint32_t get_depth_cubemap() const { return m_depth_cubemap; }
    [[nodiscard]] uint32_t get_shadow_width() const { return m_shadow_width; }
    [[nodiscard]] uint32_t get_shadow_height() const { return m_shadow_height; }

private:
    void setup_buffers();

    uint32_t m_shadow_width = 1024;
    uint32_t m_shadow_height = 1024;

    uint32_t m_depth_map_fbo = 0;
    uint32_t m_depth_cubemap = 0;
};

}// namespace engine::graphics

#endif// ENGINE_GRAPHICS_POINT_SHADOWS_HPP