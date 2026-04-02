#define GLM_ENABLE_EXPERIMENTAL
#include <engine/core/Engine.hpp>
#include "MainApp.hpp"




/**
 * Start here...
 */


int main(int argc, char **argv) {
    return std::make_unique<MainApp>()->run(argc, argv);
}
