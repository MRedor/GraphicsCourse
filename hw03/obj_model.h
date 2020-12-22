#pragma once

#include <memory>
#include "opengl_shader.h"

struct IObjModel
{
    virtual ~IObjModel(){}
    virtual void draw() = 0;
};

std::shared_ptr<IObjModel> create_model(char const * fileaname);
