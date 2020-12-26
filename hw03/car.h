#pragma once

#include "utils.h"

struct render_target_t {
    render_target_t(int res_x, int res_y);

    ~render_target_t();

    GLuint fbo_;
    GLuint color_, depth_;
    int width_, height_;
};

render_target_t::render_target_t(int res_x, int res_y) {
    width_ = res_x;
    height_ = res_y;

    glGenTextures(1, &color_);
    glBindTexture(GL_TEXTURE_2D, color_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, res_x, res_y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glGenTextures(1, &depth_);
    glBindTexture(GL_TEXTURE_2D, depth_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, res_x, res_y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                 nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &fbo_);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           color_,
                           0);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           depth_,
                           0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

render_target_t::~render_target_t() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &depth_);
    glDeleteTextures(1, &color_);
}

class Car {
public:
    Car() {
        model = create_model("../assets/car2.obj");
        render_target_t rt(512, 512);
        shader = new shader_t("model.vs", "model.fs");
        load_texture(texture, "../assets/grey.jpg");
    }

    void Draw() {
        //shader->use();
        //shader->set_uniform("u_model", glm::value_ptr(model));
        //shader->set_uniform("u_mvp", glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, texture);
        model->draw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::vec3 center() {
        return glm::vec3((model->max_x + model->min_x) / 2,(model->max_y + model->min_y) / 2,(model->max_z + model->min_z) / 2);
    }

    GLuint texture;
    shader_t* shader;
    std::shared_ptr<IObjModel> model;
};
