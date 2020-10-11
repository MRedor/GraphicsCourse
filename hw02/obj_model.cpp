#include "obj_model.h"

#include <glm/glm.hpp>

#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <GL/glew.h>


class obj_model_t : public IObjModel
{
public:
    obj_model_t(char const * filename);
    ~obj_model_t() = default;

    void draw() override;
private:
    int num_vertices_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
};


obj_model_t::obj_model_t(char const * filename)
{
    spdlog::info("Loading model: {}", filename);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;


    auto parent = fs::absolute(fs::path(filename)).parent_path();
    //const std::string& inputfile = filename;
    std::string err;
    const bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, parent.string().c_str(), true);

    if (!err.empty()) {
        spdlog::info(err, filename);
        throw std::runtime_error(fmt::format("Model load error: {}", err));
    }

    spdlog::info("buffer_data", filename);
    std::vector<float> buffer_data;

    for (auto const & shape : shapes)
    {
        for (int i = 0; i < shape.mesh.indices.size(); ++i)
        {
            const auto vx = attrib.vertices[3 * shape.mesh.indices[i].vertex_index];
            const auto vy = attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 1];
            const auto vz = attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 2];
            auto nx = attrib.normals[3 * shape.mesh.indices[i].vertex_index];
            auto ny = attrib.normals[3 * shape.mesh.indices[i].vertex_index + 1];
            auto nz = attrib.normals[3 * shape.mesh.indices[i].vertex_index + 2];
            float data[] = {vx, vy, vz, nx, ny, nz};
            buffer_data.insert(buffer_data.end(), data, data + 6);
        }
    }

    num_vertices_ = buffer_data.size() / 6;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(float), &(buffer_data[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));

}

void obj_model_t::draw()
{
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices_);
    glBindVertexArray(0);
}

std::shared_ptr<IObjModel> create_model(char const * filename)
{
    return std::static_pointer_cast<IObjModel>(std::make_shared<obj_model_t>(filename));
}