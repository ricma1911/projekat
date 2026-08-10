//
// Created by dusan-maric on 4/4/26.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP

#include "WorldSettings.hpp"
#include "engine/graphics/PointShadows.hpp"
#include "engine/resources/Shader.hpp"
#include <engine/core/Controller.hpp>
#include <engine/graphics/Bloom.hpp>


enum class TransitionState {
    IDLE,
    FADING_OUT,
    FADING_IN
};

class MainController : public engine::core::Controller {
public:
    std::string_view name() const override;

private:
    WorldSettings m_worldSettings;
    bool m_isTabPressedLastFrame = false;
    float m_lightIntensity = 5.0f;
    void initialize() override;
    bool loop() override;
    void update_camera();
    void update() override;
    void setup_spot_light(engine::resources::Shader* shader);
    void setup_point_lights(engine::resources::Shader* shader);
    void draw_car();
    void draw_side_objects();
    void draw_point_lamps();
    void draw_street_lamp();
    void draw_platform();
    void draw_skybox();
    void begin_draw() override;
    void draw() override;
    void end_draw() override;
    void render_scene_objects(engine::resources::Shader* shader);

    TransitionState m_transitionState = TransitionState::IDLE;
    float m_transitionTimer = 0.0f;
    float m_ambientFactor = 1.0f;
    engine::graphics::Bloom m_bloom;
    engine::graphics::PointShadows m_pointShadows[2];
};

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
