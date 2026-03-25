#define GLM_ENABLE_EXPERIMENTAL
#include <engine/core/Engine.hpp>
#include "FerrariController.hpp"
#include <engine/resources/ResourcesController.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/projection.hpp"



/**
 * Start here...
 */

class MojProjekat : public engine::core::App {
public:

    void app_setup() override {
        register_controller<FerrariController>();
    };
};
int main(int argc, char **argv) {
    MojProjekat projekat;
    return projekat.run(argc, argv);
}
