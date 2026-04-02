#include "MainApp.hpp"
#include "FerrariController.hpp"

void MainApp::app_setup() {
    register_controller<FerrariController>();
}