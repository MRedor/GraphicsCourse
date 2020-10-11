#pragma optimize("", off)

#include <iostream>
#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// STB, load images
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// Math constant and routines for OpenGL interop
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_shader.h"

static void glfw_error_callback(int error, const char *description)
{
   std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

void create_triangle(GLuint &vbo, GLuint &vao, GLuint &ebo)
{
   // create the triangle
   float triangle_vertices[] = {
       -1, 1, 0,	// position vertex 1
       0, 1, 0.0f,	 // color vertex 1

       -1, -1, 0.0f,  // position vertex 1
       0, 0, 0.0f,	 // color vertex 1

       1, -1, 0.0f, // position vertex 1
       1, 0, 0,	 // color vertex 1

       1, 1, 0.0f, // position vertex 1
       1, 1, 0,	 // color vertex 1
   };
   unsigned int triangle_indices[] = {
       0, 1, 2, 0, 3, 2 };
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

void load_image(GLuint & texture) {
   int width, height, channels;
   stbi_set_flip_vertically_on_load(true);
   unsigned char *image = stbi_load("grad.jpeg", &width, &height, &channels,STBI_rgb);

   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_1D, texture);
   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, width, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
   glGenerateMipmap(GL_TEXTURE_1D);

   stbi_image_free(image);
}

shader_t* triangle_shader = nullptr;

float zoom = 100.0;
float mouseX = 0.0;
float mouseY = 0.0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset == 0) {
        return;
    }

    float w = 1280.0;
    float h = 720.0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    double dx = (xpos - w/2) / zoom - mouseX;
    double dy = (h/2 - ypos) / zoom - mouseY;
    mouseX = -dx;
    mouseY = -dy;
    if (yoffset > 0) {
        zoom *= 1.1;
    } else {
        zoom /= 1.1;
    }
    dx = (xpos - w/2) / zoom;
    dy = (h/2 - ypos) / zoom;
    mouseX += dx;
    mouseY += dy;

    triangle_shader->set_uniform("zoom", zoom);
    triangle_shader->set_uniform("offset", mouseX, mouseY);
}

bool dragging = false;
double oldX, oldY;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &oldX, &oldY);
        dragging = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragging = false;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (dragging) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        mouseX += (xpos - oldX) / zoom;
        mouseY += (oldY - ypos) / zoom;
        oldX = xpos;
        oldY = ypos;

        triangle_shader->set_uniform("offset", mouseX, mouseY);
    }
}

int main(int, char **)
{
   // Use GLFW to create a simple window
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit())
      return 1;


   // GL 3.3 + GLSL 330
   const char *glsl_version = "#version 330";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

   // Create window with graphics context
   GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
   if (window == NULL)
      return 1;
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // Enable vsync

   glfwSetCursorPosCallback(window, cursor_position_callback);
   glfwSetMouseButtonCallback(window, mouse_button_callback);
   glfwSetScrollCallback(window, scroll_callback);

   // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
   if (glewInit() != GLEW_OK)
   {
      std::cerr << "Failed to initialize OpenGL loader!\n";
      return 1;
   }

   GLuint texture;
   load_image(texture);

   // create our geometries
   GLuint vbo, vao, ebo;
   create_triangle(vbo, vao, ebo);

   // init shader
   //shader_t triangle_shader("simple-shader.vs", "simple-shader.fs");
   triangle_shader = new shader_t("simple-shader.vs", "simple-shader.fs");

   // Setup GUI context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);
   ImGui::StyleColorsDark();

   while (!glfwWindowShouldClose(window))
   {
      glfwPollEvents();

      // Get windows size
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);

      // Set viewport to fill the whole window area
      glViewport(0, 0, display_w, display_h);

      // Fill background with solid color
      glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
      glClear(GL_COLOR_BUFFER_BIT);
      //glEnable(GL_CULL_FACE);

      // Gui start new frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // GUI
      ImGui::Begin("");
      static float rotation = 0.0;
      static int steps = 10;
      ImGui::SliderInt("steps", &steps, 10, 1000);
      ImGui::End();

      // Pass the parameters to the shader as uniforms
      //triangle_shader.set_uniform("u_translation", translation[0], translation[1]);
      triangle_shader->set_uniform("u_tex", int(0));
      triangle_shader->set_uniform("screenSize", 1280.0f, 720.0f);
      triangle_shader->set_uniform("steps", steps);
      triangle_shader->set_uniform("zoom", zoom);
      triangle_shader->set_uniform("mouse", mouseX, mouseY);


      // Bind triangle shader
      triangle_shader->use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_1D, texture);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // Bind vertex array = buffers + indices
      glBindVertexArray(vao);
      // Execute draw call
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindTexture(GL_TEXTURE_1D, 0);
      glBindVertexArray(0);

      // Generate gui render commands
      ImGui::Render();

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
