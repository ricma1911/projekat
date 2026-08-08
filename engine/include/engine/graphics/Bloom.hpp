#ifndef ENGINE_GRAPHICS_BLOOM_HPP
#define ENGINE_GRAPHICS_BLOOM_HPP

#include <cstdint>
#include <engine/resources/Shader.hpp>

namespace engine::graphics {

class Bloom {
public:
    Bloom() = default;
    ~Bloom();

    void init(uint32_t width, uint32_t height);
    void bind();
    void unbind();
    void render(resources::Shader* blurShader, resources::Shader* finalShader);
    void resize(uint32_t width, uint32_t height);

private:
    void setup_buffers();
    void render_quad();

    uint32_t m_width = 800;
    uint32_t m_height = 600;

    uint32_t m_hdrFBO = 0;
    uint32_t m_colorBuffers[2] = {0, 0};
    uint32_t m_rboDepth = 0;

    uint32_t m_pingpongFBO[2] = {0, 0};
    uint32_t m_pingpongColorbuffers[2] = {0, 0};

    uint32_t m_quadVAO = 0;
    uint32_t m_quadVBO = 0;
};

} // namespace engine::graphics

#endif // ENGINE_GRAPHICS_BLOOM_HPP