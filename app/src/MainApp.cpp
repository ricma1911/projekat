#include "MainApp.hpp"
#include "FerrariController.hpp"
#include "MainController.hpp"

void MainApp::app_setup() {
    auto main_controller = register_controller<MainController>();
    register_controller<FerrariController>();

    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}