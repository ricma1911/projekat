//
// Created by dusan-maric on 4/4/26.
//

#include "MainController.hpp"
#include <engine/platform/PlatformController.hpp>

std::string_view MainController::name() const {
    return "App::MainController";
}
void MainController::initialize() {
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down())
        return false;

    return true;
}

