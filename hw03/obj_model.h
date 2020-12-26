#pragma once

#include <memory>
#include "opengl_shader.h"

struct IObjModel
{
    virtual ~IObjModel(){}
    virtual void draw() = 0;

    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();

    float max_x = std::numeric_limits<float>::min();
    float max_y = std::numeric_limits<float>::min();
    float max_z = std::numeric_limits<float>::min();
};

std::shared_ptr<IObjModel> create_model(char const * fileaname);
