//
// Created by dusan-maric on 4/4/26.
//

#include "MainController.hpp"

#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "spdlog/spdlog.h"

#include <engine/platform/PlatformController.hpp>

class MainPlatformEvenetObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEvenetObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}
std::string_view MainController::name() const {
    return "App::MainController";
}
void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEvenetObserver>());
    engine::graphics::OpenGL::enable_depth_testing();

    m_bloom.init(platform->window()->width(), platform->window()->height());
    m_point_shadows[0].init(1024, 1024);
    m_point_shadows[1].init(1024, 1024);
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down())
        return false;

    return true;
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto dt = platform->dt();
    auto camera = graphics->camera();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }

    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }

    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }

    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
}

void MainController::update() {
    update_camera();

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
        m_light_intensity += 5.0f * dt;
        if (m_light_intensity > 30.0f) m_light_intensity = 30.0f;
    }

    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
        m_light_intensity -= 5.0f * dt;
        if (m_light_intensity < 0.0f) m_light_intensity = 0.0f;
    }

    bool isTabDown = platform->key(engine::platform::KeyId::KEY_TAB).is_down();
    if (isTabDown && !m_is_tab_pressed_last_frame && m_transition_state == TransitionState::IDLE) {
        m_transition_state = TransitionState::FADING_OUT;
        m_transition_timer = 0.0f;
    }
    m_is_tab_pressed_last_frame = isTabDown;

    if (m_transition_state != TransitionState::IDLE) {
        m_transition_timer += dt;

        if (m_transition_state == TransitionState::FADING_OUT) {
            float progress = m_transition_timer / 1.0f;
            m_ambient_factor = glm::mix(1.0f, 0.0f, glm::clamp(progress, 0.0f, 1.0f));

            if (m_transition_timer >= 1.0f) {
                m_ambient_factor = 0.0f;

                m_world_settings.switch_world();

                m_transition_state = TransitionState::FADING_IN;
                m_transition_timer = 0.0f;

                return;
            }
        } else if (m_transition_state == TransitionState::FADING_IN) {
            float progress = m_transition_timer / 1.0f;
            m_ambient_factor = glm::mix(0.0f, 1.0f, glm::clamp(progress, 0.0f, 1.0f));

            if (m_transition_timer >= 1.0f) {
                m_ambient_factor = 1.0f;
                m_transition_state = TransitionState::IDLE;
            }
        }
    }
}

void MainController::terminate() {
    m_bloom.terminate();
    m_point_shadows[0].terminate();
    m_point_shadows[1].terminate();
}

void MainController::setup_spot_light(engine::resources::Shader *shader) {
    shader->use();

    glm::vec3 lamp_pos = m_world_settings.active_config().spotLightPosition;
    glm::vec3 bulb_world_pos = lamp_pos + glm::vec3(0.22f, 1.57f, 0.35f);

    shader->set_vec3("spotLight.position", bulb_world_pos);
    shader->set_vec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader->set_vec3("spotLight.color", glm::vec3(m_light_intensity) * m_world_settings.active_config().spotLightColor * m_ambient_factor);

    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(30.0f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(40.0f)));

    shader->set_float("spotLight.constant", 0.7f);
    shader->set_float("spotLight.linear", 0.0001f);
    shader->set_float("spotLight.quadratic", 0.0005f);
}

void MainController::setup_point_lights(engine::resources::Shader *shader) {
    shader->use();

    shader->set_vec3("pointLights[0].position", m_world_settings.active_config().pointLight0Position);
    shader->set_vec3("pointLights[0].color", glm::vec3(m_light_intensity) * m_world_settings.active_config().pointLight0Color * m_ambient_factor);
    shader->set_float("pointLights[0].constant", 0.05f);
    shader->set_float("pointLights[0].linear", 0.09f);
    shader->set_float("pointLights[0].quadratic", 0.032f);

    shader->set_vec3("pointLights[1].position", m_world_settings.active_config().pointLight1Position);
    shader->set_vec3("pointLights[1].color", glm::vec3(m_light_intensity) * m_world_settings.active_config().pointLight1Color * m_ambient_factor);
    shader->set_float("pointLights[1].constant", 0.05f);
    shader->set_float("pointLights[1].linear", 0.09f);
    shader->set_float("pointLights[1].quadratic", 0.032f);
}

void MainController::draw_car() {
    spdlog::debug("MainController::draw_car()");
    //Treba nam model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Model *car = resources->model(m_world_settings.active_config().carModelName);
    //Treba nam shader
    engine::resources::Shader *shader = resources->shader("car");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.5f, -15.0f));
    model = glm::scale(model, glm::vec3(m_world_settings.active_config().carModelScale));
    shader->set_mat4("model", model);
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f) * m_ambient_factor);

    car->draw(shader);
}

void MainController::draw_side_objects() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Model *side_objects = resources->model(m_world_settings.active_config().sideObjectsModelName);


    engine::resources::Shader *shader = resources->shader("textured_model");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f) * m_ambient_factor);
    shader->set_bool("useEmissive", false);

    const auto &positions = m_world_settings.active_config().sideObjectPositions;

    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(positions[i]));
        model = glm::scale(model, glm::vec3(m_world_settings.active_config().sideObjectsModelScale));

        shader->set_mat4("model", model);
        side_objects->draw(shader);
    }
}

void MainController::draw_street_lamp() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *street_lamp = resources->model("street_lamp");
    engine::resources::Model *bulb = resources->model("bulb");

    engine::resources::Shader *textured_shader = resources->shader("textured_model");
    textured_shader->use();

    textured_shader->set_mat4("projection", graphics->projection_matrix());
    textured_shader->set_mat4("view", graphics->camera()->view_matrix());
    textured_shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f) * m_ambient_factor);
    textured_shader->set_bool("useEmissive", false);

    glm::vec3 lamp_pos = m_world_settings.active_config().spotLightPosition;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lamp_pos);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.0175f, 0.0175f, 0.0175f));

    textured_shader->set_mat4("model", model);
    street_lamp->draw(textured_shader);

    engine::resources::Shader *one_color_shader = resources->shader("one_color");
    one_color_shader->use();

    one_color_shader->set_mat4("projection", graphics->projection_matrix());
    one_color_shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 bulb_model = glm::mat4(1.0f);
    bulb_model = glm::translate(bulb_model, lamp_pos + glm::vec3(0.22f, 1.57f, 0.35f));
    bulb_model = glm::scale(bulb_model, glm::vec3(0.33f, 0.17f, 0.33f));

    one_color_shader->set_mat4("model", bulb_model);
    one_color_shader->set_vec3("color", m_world_settings.active_config().spotLightColor * m_ambient_factor);

    bulb->draw(one_color_shader);
}


void MainController::draw_point_lamps() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *point_lamp = resources->model("point_lamp");  //I need model
    engine::resources::Shader *shader = resources->shader("textured_model");//I need shader

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f) * m_ambient_factor);
    shader->set_bool("useEmissive", true);


    glm::vec3 positions[] = {
            m_world_settings.active_config().pointLight0Position,
            m_world_settings.active_config().pointLight1Position,
    };

    glm::vec3 colors[] = {
            m_world_settings.active_config().pointLight0Color,
            m_world_settings.active_config().pointLight1Color,
    };

    for (int i = 0; i < 2; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(positions[i]));

        shader->set_mat4("model", model);
        shader->set_vec3("emissiveColor", colors[i] * m_ambient_factor);
        point_lamp->draw(shader);
    }
}


void MainController::draw_platform() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *plane = resources->model("plane");
    engine::resources::Shader *shader = resources->shader("textured_model");

    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.5f, -15.0f));
    model = glm::scale(model, glm::vec3(3.5f, 1.0f, 3.5f));

    shader->set_mat4("model", model);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f) * m_ambient_factor);
    shader->set_bool("useEmissive", false);

    m_point_shadows[0].bind_depth_map(5);
    m_point_shadows[1].bind_depth_map(6);
    shader->set_int("depthMaps[0]", 5);
    shader->set_int("depthMaps[1]", 6);
    shader->set_float("far_plane", 25.0f);

    plane->draw(shader);
}
void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox);
}

void MainController::begin_draw() {
    spdlog::debug("MainController::begin_draw()");
    m_bloom.bind();
}


void MainController::draw() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    float farPlane = 25.0f;
    float nearPlane = 0.1f;


    auto shadowShader = resources->shader("point_shadows");
    shadowShader->use();
    shadowShader->set_float("far_plane", farPlane);

    glm::vec3 lightPositions[2] = {
            m_world_settings.active_config().pointLight0Position,
            m_world_settings.active_config().pointLight1Position};

    for (int lightIdx = 0; lightIdx < 2; ++lightIdx) {
        shadowShader->set_vec3("lightPos", lightPositions[lightIdx]);
        auto shadowMatrices = m_point_shadows[lightIdx].calculate_light_space_matrices(lightPositions[lightIdx], nearPlane, farPlane);

        for (uint32_t face = 0; face < 6; ++face) {
            m_point_shadows[lightIdx].bind_face(face);
            shadowShader->set_mat4("shadowMatrix", shadowMatrices[face]);

            render_scene_objects(shadowShader);
        }
        m_point_shadows[lightIdx].unbind(platform->window()->width(), platform->window()->height());
    }


    m_bloom.bind();

    m_point_shadows[0].bind_depth_map(5);
    m_point_shadows[1].bind_depth_map(6);

    auto carShader = resources->shader("car");
    auto texturedShader = resources->shader("textured_model");

    carShader->use();
    carShader->set_int("depthMaps[0]", 5);
    carShader->set_int("depthMaps[1]", 6);
    carShader->set_float("far_plane", farPlane);
    setup_spot_light(carShader);
    setup_point_lights(carShader);

    texturedShader->use();
    texturedShader->set_int("depthMaps[0]", 5);
    texturedShader->set_int("depthMaps[1]", 6);
    texturedShader->set_float("far_plane", farPlane);
    setup_spot_light(texturedShader);
    setup_point_lights(texturedShader);

    draw_car();
    draw_side_objects();
    draw_street_lamp();
    draw_point_lamps();
    draw_skybox();
    draw_platform();

    auto blurShader = resources->shader("blur");
    auto finalShader = resources->shader("bloom_final");
    m_bloom.render(blurShader, finalShader);
}

void MainController::end_draw() {
    spdlog::debug("MainController::end_draw()");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::render_scene_objects(engine::resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model *plane = resources->model("plane");
    glm::mat4 planeModel = glm::mat4(1.0f);
    planeModel = glm::translate(planeModel, glm::vec3(0.0f, -2.5f, -15.0f));
    planeModel = glm::scale(planeModel, glm::vec3(3.5f, 1.0f, 3.5f));
    shader->set_mat4("model", planeModel);
    plane->draw(shader);

    engine::resources::Model *car = resources->model(m_world_settings.active_config().carModelName);
    glm::mat4 carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, glm::vec3(0.0f, -2.5f, -15.0f));
    carModel = glm::scale(carModel, glm::vec3(m_world_settings.active_config().carModelScale));
    shader->set_mat4("model", carModel);
    car->draw(shader);

    engine::resources::Model *side_objects = resources->model(m_world_settings.active_config().sideObjectsModelName);
    const auto &positions = m_world_settings.active_config().sideObjectPositions;
    for (int i = 0; i < 4; i++) {
        glm::mat4 sideModel = glm::mat4(1.0f);
        sideModel = glm::translate(sideModel, glm::vec3(positions[i]));
        sideModel = glm::scale(sideModel, glm::vec3(m_world_settings.active_config().sideObjectsModelScale));
        shader->set_mat4("model", sideModel);
        side_objects->draw(shader);
    }
}
