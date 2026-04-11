#include "MainApp.hpp"
#include "MainController.hpp"

void MainApp::app_setup() {
    auto main_controller = register_controller<MainController>();

    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}