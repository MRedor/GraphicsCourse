cmake_minimum_required(VERSION 3.0)
project(opengl-imgui-sample CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
find_package(stb CONFIG)
find_package(tinyobjloader CONFIG)
find_package(spdlog CONFIG)

add_executable( opengl-imgui-sample
                main.cpp
                opengl_shader.cpp
                opengl_shader.h
                obj_model.cpp
                obj_model.h
                bindings/imgui_impl_glfw.cpp
                bindings/imgui_impl_opengl3.cpp
                bindings/imgui_impl_glfw.h
                bindings/imgui_impl_opengl3.h
                assets/skybox.vs
                assets/skybox.fs
                assets/Stairs/posx.jpg
                assets/Stairs/posy.jpg
                assets/Stairs/posz.jpg
                assets/Stairs/negx.jpg
                assets/Stairs/negy.jpg
                assets/Stairs/negz.jpg
                assets/model.vs
                assets/model.fs)

add_custom_command(TARGET opengl-imgui-sample
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/skybox.vs ${PROJECT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/skybox.fs ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/Stairs/posx.jpg ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/Stairs/posy.jpg ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/Stairs/posz.jpg ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/Stairs/negx.jpg ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/Stairs/negy.jpg ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/Stairs/negz.jpg ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/model.vs ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/model.fs ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/sphere.obj ${PROJECT_BINARY_DIR}
)
set_target_properties(opengl-imgui-sample PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET opengl-imgui-sample PROPERTY CXX_STANDARD 17)

target_compile_definitions(opengl-imgui-sample PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(opengl-imgui-sample imgui::imgui GLEW::glew_s glfw::glfw fmt::fmt glm::glm stb::stb tinyobjloader::tinyobjloader spdlog::spdlog)
