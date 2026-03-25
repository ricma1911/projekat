#include "FerrariController.hpp"


std::string_view FerrariController::name() const {
        return "FerrariController";
    }

void FerrariController::draw() {
    engine::resources::ResourcesController* res = engine::core::Controller::get<engine::resources::ResourcesController>();

    auto* ferrari_model = res->model("ferrari");
    auto* ferrari_shader = res->shader("textured_model");

    if (ferrari_model && ferrari_shader) {
        ferrari_shader->use();
        ferrari_model->draw(ferrari_shader);
    }
}
