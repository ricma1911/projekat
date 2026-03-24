#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/projection.hpp"


#include <engine/core/Engine.hpp>

/**
 * Start here...
 */


class MojProjekat : public engine::core::App {
public:

    void app_setup() override {

    };
};
int main(int argc, char **argv) {
    MojProjekat projekat;
    return projekat.run(argc, argv);
}
