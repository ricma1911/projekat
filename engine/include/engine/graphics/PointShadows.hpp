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

    void init(uint32_t shadowWidth = 1024, uint32_t shadowHeight = 1024);

    void bind();
    void unbind(uint32_t currentWindowWidth, uint32_t currentWindowHeight);

    std::vector<glm::mat4> calculate_light_space_matrices(const glm::vec3 &lightPos, float nearPlane, float farPlane);

    void bind_depth_map(uint32_t textureUnit = 0) const;
    void bind_face(uint32_t faceIndex);

    [[nodiscard]] uint32_t get_depth_cubemap() const { return m_depthCubemap; }
    [[nodiscard]] uint32_t get_shadow_width() const { return m_shadowWidth; }
    [[nodiscard]] uint32_t get_shadow_height() const { return m_shadowHeight; }

private:
    void setup_buffers();

    uint32_t m_shadowWidth = 1024;
    uint32_t m_shadowHeight = 1024;

    uint32_t m_depthMapFBO = 0;
    uint32_t m_depthCubemap = 0;
};

}// namespace engine::graphics

#endif// ENGINE_GRAPHICS_POINT_SHADOWS_HPP