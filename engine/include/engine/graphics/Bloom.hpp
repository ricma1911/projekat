#ifndef ENGINE_GRAPHICS_BLOOM_HPP
#define ENGINE_GRAPHICS_BLOOM_HPP

#include <cstdint>
#include <engine/resources/Shader.hpp>

namespace engine::graphics {

class Bloom {
public:
    Bloom() = default;

    void init(uint32_t width, uint32_t height);
    void terminate();
    void bind();
    void unbind();
    void render(resources::Shader *blur_shader, resources::Shader *final_shader);
    void resize(uint32_t width, uint32_t height);

private:
    void setup_buffers();
    void render_quad();

    uint32_t m_width = 800;
    uint32_t m_height = 600;

    uint32_t m_hdr_fbo = 0;
    uint32_t m_color_buffers[2] = {0, 0};
    uint32_t m_rbo_depth = 0;

    uint32_t m_pingpong_fbo[2] = {0, 0};
    uint32_t m_pingpong_colorbuffers[2] = {0, 0};

    uint32_t m_quad_vao = 0;
    uint32_t m_quad_vbo = 0;
};

}// namespace engine::graphics

#endif// ENGINE_GRAPHICS_BLOOM_HPP