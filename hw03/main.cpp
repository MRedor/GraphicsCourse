#include "obj_model.h"
#include "skybox.h"
#include "car.h"
#include "torus.h"

static void glfw_error_callback(int error, const char *description) {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

auto rotation = glm::mat4(1.0);
float zoom = 1;

void moveCam(ImVec2 delta) {
    float x_rotation = delta.y / 200.0;
    float y_rotation = -delta.x / 200.0;
    auto rotationX = glm::rotate(glm::mat4(1.0), glm::radians(x_rotation * 60), glm::vec3(1, 0, 0));
    auto rotationY = glm::rotate(glm::mat4(1.0), glm::radians(y_rotation * 60), glm::vec3(0, 1, 0));
    rotation = rotation * rotationX * rotationY;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    zoom = zoom * pow(0.1, yoffset);
}

float car_rotation = 0;
glm::vec2 car_position = glm::vec2(50, 50);
glm::vec2 car_direction = glm::vec2(1, 0);

void move() {
    car_position -= car_direction;

    if (car_position[0] >= 500) {
        car_position[0] = 0;
    }
    if (car_position[1] >= 500) {
        car_position[1] = 0;
    }
    if (car_position[0] < 0) {
        car_position[0] = 500;
    }
    if (car_position[1] < 0) {
        car_position[1] = 500;
    }
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_UP) {
        move();
    }

    if (key == GLFW_KEY_LEFT) {
        car_rotation += 0.1;
    }
    if (key == GLFW_KEY_RIGHT) {
        car_rotation -= 0.1;
    }

    if (abs(car_rotation) > 2 * PI_) {
        car_rotation = 0;
    }
    car_direction = glm::vec2(cos(car_rotation), sin(car_rotation));
}

glm::vec2 direction() {
    return glm::normalize(car_direction);
}

int main(int, char **) {
    // Use GLFW to create a simple window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // GL 3.3 + GLSL 330
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
    if (window == NULL) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetKeyCallback(window, keyboard_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto skybox = SkyBox();
    auto car = Car();
    auto torus = Torus(15, 8);
    GLuint texture_torus;
    load_texture(texture_torus, "../assets/grass2.jpg");
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Get windows size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Set viewport to fill the whole window area
        glViewport(0, 0, display_w, display_h);

        // Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Generate gui render commands
        ImGui::Render();

        ImVec2 delta = ImGui::GetMouseDragDelta();
        ImGui::ResetMouseDragDelta();
        moveCam(delta);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        auto camera = glm::vec3(glm::scale(rotation, glm::vec3(zoom)) * glm::vec4(0, 5, 5, 0));
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            skybox.Draw(camera, rotation);

            auto model_torus = glm::scale(glm::vec3(0.8, 0.8, 0.8));
            auto model_car_basic = glm::rotate(ALPHA, z_basic) * glm::rotate(ALPHA, x_basic) * glm::rotate(PI_, z_basic) * glm::scale(glm::vec3(0.008, 0.008, 0.008));

            auto translation = torus.get_translation_matrix(car_position);
            auto model_camera = translation * glm::rotate(car_rotation, x_basic) * model_car_basic;
            glm::vec3 camera = glm::vec3(translation * glm::vec4(3, direction().x, direction().y, 1));
            auto view = glm::lookAt(
                    camera,
                    glm::vec3(translation * glm::vec4(0, 0, 0, 1)),
                    glm::vec3(model_camera * glm::vec4(-1, 0, 0, 0))
            );

            auto projection = glm::perspective(glm::radians(90.0f), (float) display_w / (float) display_h, 0.1f,100.0f);

            glActiveTexture(GL_TEXTURE0 + texture_torus);
            glBindTexture(GL_TEXTURE_2D, texture_torus);
            torus.shader->use();
            torus.shader->set_uniform("model", glm::value_ptr(model_torus));
            torus.shader->set_uniform("view", glm::value_ptr(view));
            torus.shader->set_uniform("projection", glm::value_ptr(projection));
            torus.shader->set_uniform("tex", (int) texture_torus);
            torus.Draw();

//--------------------
            auto normal = glm::orientation(glm::vec3(model_torus * glm::vec4(torus.get_normal(car_position[0], car_position[1]), 1)), x_basic);
            glm::vec3 new_x = normalize(glm::vec3(normal * glm::vec4(1, 0, 0, 0)));
            glm::vec3 new_z = normalize(glm::vec3(normal * glm::vec4(0, 0, 1, 0)));
            glm::vec3 torus_dir_x = normalize(torus.vertex(car_position[0] + 1, car_position[1]) - torus.vertex(car_position[0], car_position[1]));
            glm::vec3 torus_dir_y = normalize(torus.vertex(car_position[0], car_position[1] + 1) - torus.vertex(car_position[0], car_position[1]));

            auto angle = acos(dot(new_z, torus_dir_x));
            if (dot(new_z, torus_dir_y) > 0) {
                angle = -angle;
            }
            auto landscape_rotation = glm::rotate(angle, new_x);
//--------------------
            auto rotation = glm::rotate(car_rotation - ALPHA, new_x);
            auto car_translation = glm::translate(glm::vec3(model_torus * glm::vec4(torus.vertex(car_position), 1))
                        - glm::vec3(model_car_basic * glm::vec4(car.center(), 1)));

            auto model_car = car_translation * rotation * landscape_rotation * normal * model_car_basic;
            auto mvp_car = projection * view * model_car;

            car.shader->use();
            car.shader->set_uniform("u_model", glm::value_ptr(model_car));
            car.shader->set_uniform("u_mvp", glm::value_ptr(mvp_car));
            car.Draw();
        }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Execute gui render commands using OpenGL backend
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Swap the backbuffer with the frontbuffer that is used for screen display
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}