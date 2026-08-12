#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PointShadows.hpp>

namespace engine::graphics {

void PointShadows::init(uint32_t shadow_width, uint32_t shadow_height) {
    m_shadow_width = shadow_width;
    m_shadow_height = shadow_height;
    setup_buffers();
}

void PointShadows::terminate() {
    if (m_depth_map_fbo) CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_depth_map_fbo);
    if (m_depth_cubemap) CHECKED_GL_CALL(glDeleteTextures, 1, &m_depth_cubemap);
}

void PointShadows::setup_buffers() {
    if (m_depth_map_fbo) CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_depth_map_fbo);
    if (m_depth_cubemap) CHECKED_GL_CALL(glDeleteTextures, 1, &m_depth_cubemap);

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_depth_map_fbo);

    CHECKED_GL_CALL(glGenTextures, 1, &m_depth_cubemap);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_depth_cubemap);

    for (uint32_t i = 0; i < 6; ++i) {
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                        m_shadow_width, m_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depth_map_fbo);
    CHECKED_GL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
    CHECKED_GL_CALL(glDrawBuffer, GL_NONE);
    CHECKED_GL_CALL(glReadBuffer, GL_NONE);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void PointShadows::bind() {
    CHECKED_GL_CALL(glViewport, 0, 0, m_shadow_width, m_shadow_height);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depth_map_fbo);
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

void PointShadows::unbind(uint32_t current_window_width, uint32_t current_window_height) {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, current_window_width, current_window_height);
}

std::vector<glm::mat4> PointShadows::calculate_light_space_matrices(const glm::vec3 &light_pos, float near_plane, float far_plane) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    return shadowTransforms;
}

void PointShadows::bind_depth_map(uint32_t texture_unit) const {
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0 + texture_unit);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
}

void PointShadows::bind_face(uint32_t face_index) {
    CHECKED_GL_CALL(glViewport, 0, 0, m_shadow_width, m_shadow_height);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depth_map_fbo);
    CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index, m_depth_cubemap, 0);
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

}// namespace engine::graphics