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
    void update_camera();
    void update() override;
    void draw_car();
    void draw_side_objects();
    void draw_point_lamps();
    void draw_platform();
    void draw_skybox();
    void begin_draw() override;
    void draw() override;
    void end_draw() override;
};

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
