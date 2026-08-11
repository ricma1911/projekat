#define GLM_ENABLE_EXPERIMENTAL
#include "MainApp.hpp"
#include <engine/core/Engine.hpp>


/**
 * Start here...
 */


int main(int argc, char **argv) {
    return std::make_unique<MainApp>()->run(argc, argv);
}
