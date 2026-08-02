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
}

void MainController::setup_spot_light(engine::resources::Shader* shader) {
    shader->use();

    glm::vec3 lamp_pos = glm::vec3(0.0f, -2.5f, -16.5f);
    glm::vec3 bulb_world_pos = lamp_pos + glm::vec3(0.22f, 1.57f, 0.35f);

    shader->set_vec3("spotLight.position", bulb_world_pos);
    shader->set_vec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader->set_vec3("spotLight.color", glm::vec3(0.0f, 0.94f, 1.0f));

    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(30.0f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(40.0f)));

    shader->set_float("spotLight.constant", 0.3f);
    shader->set_float("spotLight.linear", 0.0001f);
    shader->set_float("spotLight.quadratic", 0.0005f);
}

void MainController::draw_car() {
    spdlog::debug("MainController::draw_car()");
    //Treba nam model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Model* car = resources->model("ferrari");
    //Treba nam shader
    engine::resources::Shader* shader = resources->shader("car");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.5f, -15.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f));

    car->draw(shader);
}

void MainController::draw_side_objects() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Model* side_objects = resources->model("palm");


    engine::resources::Shader* shader = resources->shader("textured_model");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_bool("useEmissive", false);

    glm::vec3 positions[] = {
        glm::vec3(-2.0f, -1.6f, -17.0f),
        glm::vec3( 2.0f, -1.6f, -17.0f),
        glm::vec3(-2.0f, -1.6f, -13.0f),
        glm::vec3( 2.0f, -1.6f, -13.0f)
    };

    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(positions[i]));

        shader->set_mat4("model", model);
        side_objects->draw(shader);
    }

}

void MainController::draw_street_lamp() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* street_lamp = resources->model("street_lamp");
    engine::resources::Model* bulb = resources->model("bulb");

    engine::resources::Shader* textured_shader = resources->shader("textured_model");
    textured_shader->use();

    textured_shader->set_mat4("projection", graphics->projection_matrix());
    textured_shader->set_mat4("view", graphics->camera()->view_matrix());
    textured_shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f));
    textured_shader->set_bool("useEmissive", false);

    glm::vec3 lamp_pos = glm::vec3(0.0f, -2.5f, -16.5f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lamp_pos);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.0175f, 0.0175f, 0.0175f));

    textured_shader->set_mat4("model", model);
    street_lamp->draw(textured_shader);

    engine::resources::Shader* one_color_shader = resources->shader("one_color");
    one_color_shader->use();

    one_color_shader->set_mat4("projection", graphics->projection_matrix());
    one_color_shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 bulb_model = glm::mat4(1.0f);
    bulb_model = glm::translate(bulb_model, lamp_pos + glm::vec3(0.22f, 1.57f, 0.35f));
    bulb_model = glm::scale(bulb_model, glm::vec3(0.33f, 0.17f, 0.33f));

    one_color_shader->set_mat4("model", bulb_model);
    one_color_shader->set_vec3("color", glm::vec3(0.0f, 0.94f, 1.0f));

    bulb->draw(one_color_shader);
}


void MainController::draw_point_lamps() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* point_lamp = resources->model("point_lamp"); //I need model
    engine::resources::Shader* shader = resources->shader("textured_model"); //I need shader

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_bool("useEmissive", true);



    glm::vec3 positions[] = {
        glm::vec3(2.5f, -1.5f, -15.0f),
        glm::vec3(-2.5f, -1.5f, -15.0f)
    };

    glm::vec3 colors[] = {
        glm::vec3(1.0f, 0.0f, 0.8f),
        glm::vec3(1.0f, 0.45f, 0.0f)
    };

    for (int i = 0; i < 2; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(positions[i]));

        shader->set_mat4("model", model);
        shader->set_vec3("emissiveColor", colors[i]);
        point_lamp->draw(shader);
    }

}


void MainController::draw_platform() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* plane = resources->model("plane");
    engine::resources::Shader* shader = resources->shader("textured_model");

    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.5f, -15.0f));
    model = glm::scale(model, glm::vec3(3.5f, 1.0f, 3.5f));

    shader->set_mat4("model", model);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("globalAmbient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_bool("useEmissive", false);



    plane->draw(shader);

}
void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox);
}

void MainController::begin_draw() {
    spdlog::debug("MainController::begin_draw()");
    engine::graphics::OpenGL::clear_buffers();
}


void MainController::draw() {
    spdlog::debug("MainController::draw()");
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    setup_spot_light(resources->shader("car"));
    setup_spot_light(resources->shader("textured_model"));
    draw_car();
    draw_side_objects();
    draw_street_lamp();
    draw_point_lamps();
    draw_skybox();
    draw_platform();
}


void MainController::end_draw() {
    spdlog::debug("MainController::end_draw()");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

