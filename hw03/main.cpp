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
    glfwSetScrollCallback(window, scroll_callback);

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

        // GUI
        ImGui::Begin("");
        static float refraction = 1;
        ImGui::SliderFloat("refraction", &refraction, 1, 2.5);
        ImGui::End();
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
            auto model = glm::translate(
                    glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.1, 0.1, 0.1)),
                    glm::vec3(0, 0.2, 0)
            );
            auto view = glm::lookAt<float>(
                    camera,
                    glm::vec3(0, 0, 0),
                    glm::vec3(rotation * glm::vec4(0, 1, 0, 1))
            );
            auto projection = glm::perspective(glm::radians(90.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                               100.0f);
            auto mvp = projection * view * model;

            car.shader->use();
            car.shader->set_uniform("u_model", glm::value_ptr(model));
            car.shader->set_uniform("u_mvp", glm::value_ptr(mvp));
            //car.Draw(mvp);

            skybox.Draw(camera, rotation);

            glActiveTexture(GL_TEXTURE0 + texture_torus);
            glBindTexture(GL_TEXTURE_2D, texture_torus);
            torus.shader->use();
            torus.shader->set_uniform("model", glm::value_ptr(model));
            torus.shader->set_uniform("view", glm::value_ptr(view));
            torus.shader->set_uniform("projection", glm::value_ptr(projection));
            torus.shader->set_uniform("tex", (int) texture_torus);
            torus.Draw();
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