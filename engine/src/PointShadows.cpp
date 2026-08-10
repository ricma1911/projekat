#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PointShadows.hpp>

namespace engine::graphics {

PointShadows::~PointShadows() {
    if (m_depthMapFBO) CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_depthMapFBO);
    if (m_depthCubemap) CHECKED_GL_CALL(glDeleteTextures, 1, &m_depthCubemap);
}

void PointShadows::init(uint32_t shadowWidth, uint32_t shadowHeight) {
    m_shadowWidth = shadowWidth;
    m_shadowHeight = shadowHeight;
    setup_buffers();
}

void PointShadows::setup_buffers() {
    if (m_depthMapFBO) CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_depthMapFBO);
    if (m_depthCubemap) CHECKED_GL_CALL(glDeleteTextures, 1, &m_depthCubemap);

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_depthMapFBO);

    CHECKED_GL_CALL(glGenTextures, 1, &m_depthCubemap);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_depthCubemap);

    for (uint32_t i = 0; i < 6; ++i) {
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                        m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depthMapFBO);
    CHECKED_GL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
    CHECKED_GL_CALL(glDrawBuffer, GL_NONE);
    CHECKED_GL_CALL(glReadBuffer, GL_NONE);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void PointShadows::bind() {
    CHECKED_GL_CALL(glViewport, 0, 0, m_shadowWidth, m_shadowHeight);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depthMapFBO);
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

void PointShadows::unbind(uint32_t currentWindowWidth, uint32_t currentWindowHeight) {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, currentWindowWidth, currentWindowHeight);
}

std::vector<glm::mat4> PointShadows::calculate_light_space_matrices(const glm::vec3& lightPos, float nearPlane, float farPlane) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

    return shadowTransforms;
}

void PointShadows::bind_depth_map(uint32_t textureUnit) const {
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0 + textureUnit);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_depthCubemap);
}

void PointShadows::bind_face(uint32_t faceIndex) {
    CHECKED_GL_CALL(glViewport, 0, 0, m_shadowWidth, m_shadowHeight);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depthMapFBO);
    CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, m_depthCubemap, 0);
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
}

} // namespace engine::graphics