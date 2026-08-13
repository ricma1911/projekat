#include <glad/glad.h>

#include <engine/graphics/Bloom.hpp>
#include <engine/graphics/OpenGL.hpp>

namespace engine::graphics {


void Bloom::init(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    setup_buffers();
}

void Bloom::terminate() {
    if (m_hdr_fbo) CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_hdr_fbo);
    if (m_color_buffers[0]) CHECKED_GL_CALL(glDeleteTextures, 2, m_color_buffers);
    if (m_rbo_depth) CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &m_rbo_depth);

    if (m_pingpong_fbo[0]) CHECKED_GL_CALL(glDeleteFramebuffers, 2, m_pingpong_fbo);
    if (m_pingpong_colorbuffers[0]) CHECKED_GL_CALL(glDeleteTextures, 2, m_pingpong_colorbuffers);

    if (m_quad_vao) {
        CHECKED_GL_CALL(glDeleteVertexArrays, 1, &m_quad_vao);
        CHECKED_GL_CALL(glDeleteBuffers, 1, &m_quad_vbo);
    }
}

void Bloom::setup_buffers() {
    if (m_hdr_fbo) CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_hdr_fbo);
    if (m_color_buffers[0]) CHECKED_GL_CALL(glDeleteTextures, 2, m_color_buffers);
    if (m_rbo_depth) CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &m_rbo_depth);

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_hdr_fbo);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_fbo);

    CHECKED_GL_CALL(glGenTextures, 2, m_color_buffers);
    for (uint32_t i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_color_buffers[i], 0);
    }

    CHECKED_GL_CALL(glGenRenderbuffers, 1, &m_rbo_depth);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, m_rbo_depth);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_depth);

    uint32_t attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    if (m_pingpong_fbo[0]) CHECKED_GL_CALL(glDeleteFramebuffers, 2, m_pingpong_fbo);
    if (m_pingpong_colorbuffers[0]) CHECKED_GL_CALL(glDeleteTextures, 2, m_pingpong_colorbuffers);

    CHECKED_GL_CALL(glGenFramebuffers, 2, m_pingpong_fbo);
    CHECKED_GL_CALL(glGenTextures, 2, m_pingpong_colorbuffers);

    for (uint32_t i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_pingpong_fbo[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_pingpong_colorbuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpong_colorbuffers[i], 0);
    }
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void Bloom::resize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    if (m_width == width && m_height == height) return;

    m_width = width;
    m_height = height;
    setup_buffers();
}

void Bloom::bind() {
    GLint viewport[4];
    CHECKED_GL_CALL(glGetIntegerv, GL_VIEWPORT, viewport);
    if (viewport[2] > 0 && viewport[3] > 0) {
        resize(static_cast<uint32_t>(viewport[2]), static_cast<uint32_t>(viewport[3]));
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_fbo);
    CHECKED_GL_CALL(glViewport, 0, 0, m_width, m_height);
    OpenGL::clear_buffers();
}

void Bloom::unbind() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void Bloom::render(resources::Shader *blur_shader, resources::Shader *final_shader) {
    unbind();

    CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);
    CHECKED_GL_CALL(glDisable, GL_CULL_FACE);

    bool horizontal = true, first_iteration = true;
    uint32_t amount = 10;
    blur_shader->use();

    for (uint32_t i = 0; i < amount; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_pingpong_fbo[horizontal]);
        CHECKED_GL_CALL(glViewport, 0, 0, m_width, m_height);

        blur_shader->set_int("horizontal", horizontal);

        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, first_iteration ? m_color_buffers[1] : m_pingpong_colorbuffers[!horizontal]);
        blur_shader->set_int("image", 0);

        render_quad();

        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, m_width, m_height);

    OpenGL::clear_buffers();
    final_shader->use();

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffers[0]);
    final_shader->set_int("sceneTexture", 0);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_pingpong_colorbuffers[!horizontal]);
    final_shader->set_int("bloomBlur", 1);

    render_quad();

    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);
}

void Bloom::render_quad() {
    if (m_quad_vao == 0) {
        float quadVertices[] = {
                -1.0f,
                1.0f,
                0.0f,
                0.0f,
                1.0f,
                -1.0f,
                -1.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
                1.0f,
                0.0f,
                1.0f,
                1.0f,
                1.0f,
                -1.0f,
                0.0f,
                1.0f,
                0.0f,
        };
        CHECKED_GL_CALL(glGenVertexArrays, 1, &m_quad_vao);
        CHECKED_GL_CALL(glGenBuffers, 1, &m_quad_vbo);
        CHECKED_GL_CALL(glBindVertexArray, m_quad_vao);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_quad_vbo);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }
    CHECKED_GL_CALL(glBindVertexArray, m_quad_vao);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

}// namespace engine::graphics