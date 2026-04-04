//
// Created by dusan-maric on 4/4/26.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

class MainController : public engine::core::Controller {
public:
    std::string_view name() const override;

private:
    void initialize() override;
    bool loop() override;
};

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
