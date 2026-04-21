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
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, -3.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);

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

    glm::vec3 positions[] = {
        glm::vec3(-5.0f, -2.5f, -5.0f),
        glm::vec3( 5.0f, -2.5f, -5.0f),
        glm::vec3(-5.0f, -2.5f, -15.0f),
        glm::vec3( 5.0f, -2.5f, -15.0f)
    };

    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(positions[i]));

        shader->set_mat4("model", model);
        side_objects->draw(shader);
    }

}

void MainController::draw_platform() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* plane = resources->model("plane");
    engine::resources::Shader* shader = resources->shader("textured_model");

    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.6f, -20.0f));
    model = glm::scale(model, glm::vec3(5.0f, 1.0f, 10.0f));

    shader->set_mat4("model", model);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

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
    draw_car();
    draw_side_objects();
    draw_skybox();
    draw_platform();
}


void MainController::end_draw() {
    spdlog::debug("MainController::end_draw()");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

