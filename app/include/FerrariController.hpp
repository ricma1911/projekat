#ifndef FERRARI_CONTROLLER_HPP

#define FERRARI_CONTROLLER_HPP

#include <engine/core/Engine.hpp>
#include <engine/resources/ResourcesController.hpp>

class FerrariController : public engine::core::Controller {
public:
    std::string_view name() const override;
    void draw() override;
};

#endif