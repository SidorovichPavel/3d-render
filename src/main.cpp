#include <GL/glew.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <memory>
#include <chrono>
#include <fstream>
#include <iterator>
#include <numeric>

#include <threadpoollib/threadpool.hpp>
#include <tinyalgebralib/math/math.hpp>
#include <tinyalgebralib/Camera.hpp>
#include <glfwextlib/glfwext.hpp>
#include <glfwextlib/Window.hpp>
#include <glewextlib/glewext.hpp>
#include <glewextlib/Shader.hpp>
#include <glewextlib/Texture.hpp>

#include "Model/Model.hpp"

std::tuple<std::vector<uint32_t>, std::vector<ta::vec3>> view_frustum_culling(std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices);
void do_movement(const glfwext::Window& window, ta::Camera& camera, Model& model, float ms);

int main()
{
    std::string_view vshader("/mnt/sata0/Workshop/3d-render/resources/glsl/main.vert");
    std::string_view fshader("/mnt/sata0/Workshop/3d-render/resources/glsl/main.frag");

    ta::vec2i screen_size{ 1280, 720 };
    std::unique_ptr<glfwext::Window> window;
    std::unique_ptr<glewext::Shader> main_shader;
    try
    {
        glfwext::init(); // init GLFW3
        window = std::make_unique<glfwext::Window>("GLFW Window", screen_size.x(), screen_size.y());
        window->make_current(); // make current gl context
        glewext::init(); // init glew
        main_shader = std::make_unique<glewext::Shader>(vshader, fshader);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    window->framebuffer_resize += [](glfwext::Window* window, int width, int height) {
        glViewport(0, 0, width, height);
        };

    bool mouse_input = false;
    float lastX, lastY;
    float fovy = 90.f;

    window->key_press += [&](glfwext::Window* window, int key, int scancode, int mode) {
        if (key == GLFW_KEY_TAB)
        {
            mouse_input = !mouse_input;
            if (mouse_input)
            {
                window->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                std::tie(lastX, lastY) = window->cursor_pos();
            }
            else
                window->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        };

    window->key_press += [](glfwext::Window* window, int key, int scancode, int mode) {
        if (key == GLFW_KEY_ESCAPE)
            window->set_should_close(true);
        };

    window->scroll += [&](glfwext::Window* window, float xoffset, float yoffset)
        {
            if (!mouse_input)
                return;

            fovy += yoffset;
            if (fovy > 120.f)
                fovy = 120.f;
            if (fovy < 60.f)
                fovy = 60.f;
        };

    ta::Camera camera(ta::vec3(390.f, 0.f, 0.f), ta::vec3(0.f, 0.f, 0.f), ta::vec3(0.f, 1.f, 0.f));

    window->cursor_move += [&](glfwext::Window*, float xpos, float ypos) {
        if (!mouse_input)
            return;

        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera.update_angles(yoffset, xoffset, 0.f);
        };

    threadpool::threadpool pool(12);

    float vertices[] = {
        // Positions          // Colors           // Texture Coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    // Create texture data
    std::tuple<float, float, float, float> bgd_tuple_color(0.2f, 0.3f, 0.3f, 1.f);
    constexpr ta::vec3 bgd_color(0.2f, 0.3f, 0.3f);
    auto [width, height] = window->framebuffer_size();
    std::vector<ta::vec3> image(width * height);
    for (auto& c : image)
        c = bgd_color;
    // Load and create a texture
    glewext::Texture texture;
    texture.load_image_from_memory(glewext::TextureLevel::Base, glewext::TextureInternalFormat::RGB,
        width, height,
        glewext::TextureBorder::NoBorder, glewext::TextureFormat::RGB, glewext::GLType::Float,
        image.data());
    glewext::Texture::unbind();

    std::string_view filename("/mnt/sata0/Workshop/3d-render/resources/models/hyperion.stl");
    Model model;
    model.load_from_file(filename);
    //model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(90.f));

    auto time = std::chrono::steady_clock::now();

    for (; !window->should_close();)
    {
        auto rend_begin = std::chrono::steady_clock::now();

        glfwPollEvents();
        auto tp = std::chrono::steady_clock::now();
        auto frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(time - tp).count() / 1000.f;
        time = tp;

        do_movement(*window, camera, model, frame_time);

        auto projection = ta::perspective(ta::rad(fovy), window->ratio(), .1f, 500.f);
        auto view = camera.get_view();
        auto [tvertices, tindices] = model.transform(view, projection);

        auto [vfc_indices, vfc_vertices] = view_frustum_culling(tindices, tvertices);

        auto [width, height] = window->framebuffer_size();
        auto viewport = ta::viewport(0, 0, width, height);
        
        pool.transform(vfc_vertices, vfc_vertices.begin(), [&](const ta::vec3& vec) { return (viewport * ta::vec4(vec, 1.f)).swizzle<0, 1, 2>(); });

        // Bind Texture
        if (image.size() != width * height)
            image.resize(width * height);

        for (auto& c : image)
            c = bgd_color * 2;

        for (auto&& tri : vfc_indices | std::views::chunk(3))
        {
            auto v1 = vfc_vertices[tri[0]],
                v2 = vfc_vertices[tri[1]],
                v3 = vfc_vertices[tri[2]];

            auto dda = [&](ta::vec3 u, ta::vec3 v)
                {
                    auto distx = v.x() - u.x(),
                        disty = v.y() - u.y();

                    auto l = static_cast<size_t>(std::max(std::abs(distx), std::abs(disty)));

                    ta::vec3 d(distx / l, disty / l, 0.f);

                    for (auto step : std::views::iota(0u, l))
                    {
                        auto x = static_cast<int>(u.x()),
                            y = static_cast<int>(u.y());
                        u += d;

                        auto cx = std::clamp(x, 0, width - 1),
                            cy = std::clamp(y, 0, height - 1);

                        image[cy * width + cx] = ta::vec3(1.f);
                    }
                };

            dda(v1, v2);
            dda(v2, v3);
            dda(v3, v1);
        }

        std::apply(glClearColor, bgd_tuple_color);
        glClear(GL_COLOR_BUFFER_BIT);

        texture.load_image_from_memory(glewext::TextureLevel::Base, glewext::TextureInternalFormat::RGB,
            width, height,
            glewext::TextureBorder::NoBorder, glewext::TextureFormat::RGB, glewext::GLType::Float,
            image.data());
        texture.bind(glewext::TextureUnit::_0);
        // Activate shader
        main_shader->use();

        // Draw container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glewext::Texture::unbind();

        window->swap_buffers();
        //std::cout << /* 1000.f / */ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - rend_begin).count() << std::endl;
    }

    // Освобождение ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Завершение работы GLFW
    glfwTerminate();
    return 0;
}

void do_movement(const glfwext::Window& window, ta::Camera& camera, Model& model, float ms)
{
    if (ms == INFINITY)
        ms = 0.0001f;

    decltype(auto) keys = window.keys_state();

    float cameraSpeed = 200.f;

    if (keys[GLFW_KEY_W])
        camera.move_front(-cameraSpeed * ms);
    if (keys[GLFW_KEY_S])
        camera.move_back(-cameraSpeed * ms);
    if (keys[GLFW_KEY_A])
        camera.move_left(cameraSpeed * ms);
    if (keys[GLFW_KEY_D])
        camera.move_right(cameraSpeed * ms);

    if (keys[GLFW_KEY_UP])
        model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(15.0f * ms));
    if (keys[GLFW_KEY_DOWN])
        model.rotare(ta::vec3(1.f, 0.f, 0.f), -ta::rad(15.0f * ms));
    if (keys[GLFW_KEY_LEFT])
        model.rotare(ta::vec3(0.f, 1.f, 0.f), ta::rad(15.0f * ms));
    if (keys[GLFW_KEY_RIGHT])
        model.rotare(ta::vec3(0.f, 1.f, 0.f), -ta::rad(15.0f * ms));
}

std::tuple<std::vector<uint32_t>, std::vector<ta::vec3>> view_frustum_culling(std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices)
{
    std::vector<uint32_t>resi;
    std::vector<ta::vec3>resv;
    constexpr size_t triangle{ 3 };
    for (auto&& tri : indices | std::views::chunk(triangle))
    {
        auto v1 = vertices[tri[0]],
            v2 = vertices[tri[1]],
            v3 = vertices[tri[2]];

        auto in_normal_range = [](const ta::vec3& v)
            {
                return (
                    v.x() >= -1.f && v.x() <= 1.f &&
                    v.y() >= -1.f && v.y() <= 1.f &&
                    v.z() >= -1.f && v.z() <= 1.f);
            };

        if (in_normal_range(v1) && in_normal_range(v2) && in_normal_range(v3))
        {
            resi.insert(resi.end(), tri.begin(), tri.end());
        }
    }

    return std::make_tuple(resi, std::move(vertices));
}