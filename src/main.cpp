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

void do_movement(const glfwext::Window& window, ta::Camera& camera, Model& model, float ms);
std::vector<ta::vec2i> apply_viewport(const ta::mat4& viewport, const std::vector<ta::vec3>& vertices, threadpool::threadpool& pool);

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
    glewext::Texture tex1;
    tex1.load_image_from_memory(glewext::TextureLevel::Base, glewext::TextureInternalFormat::RGB,
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

        auto projection = ta::perspective(ta::rad(90.f), window->ratio(), .1f, 500.f);
        auto view = camera.get_view();
        auto [width, height] = window->framebuffer_size();
        auto [tdata, indices] = model.transform(view, projection);

        auto viewport = ta::viewport(0, 0, width, height);

        auto pixels = apply_viewport(viewport, tdata, pool);

        // Bind Texture
        if (image.size() != width * height)
            image.resize(width * height);

        for (auto& c : image)
            c = bgd_color * 2;

        for (auto i : std::views::iota(0u, indices.size() / 3))
        {
            auto tv1 = indices[i * 3 + 0],
                tv2 = indices[i * 3 + 1],
                tv3 = indices[i * 3 + 2];

            auto v1 = pixels[tv1],
                v2 = pixels[tv2],
                v3 = pixels[tv3];

            auto dda = [&](ta::vec2i u, ta::vec2i v)
                {
                    auto distx = v.x() - u.x(),
                        disty = v.y() - u.y();

                    auto l = static_cast<size_t>(std::max(std::abs(distx), std::abs(disty)));
                    if (l > 2000)
                        return;

                    ta::vec2 d(static_cast<float>(distx) / l, static_cast<float>(disty) / l);

                    ta::vec2 pos = u;

                    for (auto step : std::views::iota(0u, l))
                    {
                        auto x = static_cast<int>(pos.x()),
                            y = static_cast<int>(pos.y());
                        pos += d;

                        auto cx = std::clamp(x, 0, width - 1),
                            cy = std::clamp(y, 0, height - 1);

                        image[cy * width + cx] = ta::vec3(1.f);
                    }
                };

            auto f1 = pool.enqueue(dda, v1, v2);
            auto f2 = pool.enqueue(dda, v2, v3);
            auto f3 = pool.enqueue(dda, v3, v1);

            f1.get();
            f2.get();
            f3.get();
        }

        std::apply(glClearColor, bgd_tuple_color);
        glClear(GL_COLOR_BUFFER_BIT);

        tex1.load_image_from_memory(glewext::TextureLevel::Base, glewext::TextureInternalFormat::RGB,
            width, height,
            glewext::TextureBorder::NoBorder, glewext::TextureFormat::RGB, glewext::GLType::Float,
            image.data());
        tex1.bind(glewext::TextureUnit::_0);
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

std::vector<ta::vec2i> apply_viewport(const ta::mat4& viewport, const std::vector<ta::vec3>& vertices, threadpool::threadpool& pool)
{
    std::vector<ta::vec2i> result(vertices.size());

    pool.transform(vertices, result.begin(),
        [&](const ta::vec3& vec)
        {
            auto v4 = viewport * ta::vec4(vec, 1.f);

            return ta::vec2i(static_cast<int>(v4.x()), static_cast<int>(v4.y()));
        });

    return result;
}