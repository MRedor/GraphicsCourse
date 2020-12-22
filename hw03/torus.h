#pragma once

#include "utils.h"

const float PI_ = 3.14159265358979323846;
const float TAU = 2 * PI_;

class HeightsMap {
public:
    HeightsMap(char* filename) {
        int channels;
        image = stbi_load(filename, &width, &height, &channels, STBI_rgb);
    }

    float Point(float i, float j) {
        return float(image[3 * int(floor(j * width) * width + floor(i * height))]) / 255 / 2;
    }

    unsigned char* image;
    int width;
    int height;
};


class Torus {
public:

    Torus(int R, int r) : R(R), r(r) {
        load_texture(texture, "../assets/grass2.jpg");
        shader = new shader_t("../assets/torus.vs", "../assets/torus.fs");

        std::vector<float> buffer_data;
        std::vector<int> indices;

        for (int i = 0; i < numc; i++) {
            for (int j = 0; j < numt; j++) {
                auto v = vertex(i, j);
                buffer_data.emplace_back(v[0]);
                buffer_data.emplace_back(v[1]);
                buffer_data.emplace_back(v[2]);

                buffer_data.emplace_back(float(j) / numt);
                buffer_data.emplace_back(float(i) / numc);
                buffer_data.emplace_back(height(i, j));

                int i_ = (i + 1) % (numc - 1);
                int j_ = (j + 1) % (numt - 1);
                indices.emplace_back(i * numt + j);
                indices.emplace_back(i * numt + j_);
                indices.emplace_back(i_ * numt + j);
                indices.emplace_back(i * numt + j_);
                indices.emplace_back(i_ * numt + j);
                indices.emplace_back(i_ * numt + j_);
            }
        }
        num_vertices_ = indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer_data.size(), buffer_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, num_vertices_, GL_UNSIGNED_INT, 0);
    }


    shader_t* shader;

private:
    glm::vec3 vertex(int i, int j) {
        auto c = i * TAU / (numc - 1);
        auto t = j * TAU / (numt - 1) - PI_;

        auto h = height(i, j);//heights.Point(i / numc, j / numt) * r;

        auto x = (R + (r + h) * cos(t)) * cos(c);
        auto y = (R + (r + h) * cos(t)) * sin(c);
        auto z = (r + h) * sin(t);

        return {x, y, z};
    }

    float height(int i, int j) {
        return heights.Point(float(i) / numc, float(j) / numt) * r;
    }

    int numc = 100, numt = 1000;
    int R;
    int r;
    HeightsMap heights = HeightsMap("../assets/height-map.png");

    GLuint vbo, vao, ebo;
    GLuint texture;
    int num_vertices_;
};
