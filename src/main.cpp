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
#include <array>
#include <cmath>
#include <format>

#include <threadpoollib/threadpool.hpp>
#include <tinyalgebralib/math/math.hpp>
#include <tinyalgebralib/Camera.hpp>
#include <glfwextlib/glfwext.hpp>
#include <glfwextlib/Window.hpp>
#include <glewextlib/glewext.hpp>
#include <glewextlib/Shader.hpp>
#include <glewextlib/Texture.hpp>

#include "Model/Model.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"

std::tuple<std::vector<uint32_t>, std::vector<ta::vec3>> backface_culling(const ta::vec3& eye, std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices);
std::tuple<std::vector<uint32_t>, std::vector<ta::vec3>> view_frustum_culling(std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices);

void rasterization(std::tuple<int, int, int, int> image_box, ScreenBuffer& scbuffer,
    std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices);

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

    ScreenBuffer screen_buffer(width, height, ta::vec3(0.4f, 0.6f, 0.6f));

    // Load and create a texture
    glewext::Texture texture;
    texture.load_image_from_memory(glewext::TextureLevel::Base, glewext::TextureInternalFormat::RGB,
        width, height,
        glewext::TextureBorder::NoBorder, glewext::TextureFormat::RGB, glewext::GLType::Float,
        screen_buffer.data());
    glewext::Texture::unbind();

    std::string_view filename("/mnt/sata0/Workshop/3d-render/resources/models/hyperion.stl");
    Model model;
    model.load_from_file(filename);
    model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(90.f));

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
        auto [tvertices, tindices] = model.transform(view, projection, pool);


        auto [bfc_indives, bfc_vertices] = backface_culling(camera.position(), tindices, tvertices);
        auto [vfc_indices, vfc_vertices] = view_frustum_culling(bfc_indives, bfc_vertices);

        auto [width, height] = window->framebuffer_size();
        auto viewport = ta::viewport(0, 0, width, height);

        pool.transform(vfc_vertices, vfc_vertices.begin(), [&](const ta::vec3& vec) { return ta::vec3(viewport * ta::vec4(vec, 1.f)); });

        // Bind Texture
        screen_buffer.clear();

        rasterization(std::make_tuple(0, 0, width, height), screen_buffer, vfc_indices, vfc_vertices);

        std::apply(glClearColor, bgd_tuple_color);
        glClear(GL_COLOR_BUFFER_BIT);

        texture.load_image_from_memory(glewext::TextureLevel::Base, glewext::TextureInternalFormat::RGB,
            width, height,
            glewext::TextureBorder::NoBorder, glewext::TextureFormat::RGB, glewext::GLType::Float,
            screen_buffer.data());
        texture.bind(glewext::TextureUnit::_0);
        // Activate shader
        main_shader->use();

        // Draw container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glewext::Texture::unbind();

        window->swap_buffers();
        std::cout << /* 1000.f / */ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - rend_begin).count() << std::endl;
    }

    // Освобождение ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Завершение работы GLFW
    glfwTerminate();
    return 0;
}

void do_movement(const glfwext::Window& window, ta::Camera& camera, Model& model, float t)
{
    if (t == INFINITY)
        t = 0.0001f;

    decltype(auto) keys = window.keys_state();

    float cameraSpeed = 200.f;
    ta::vec3 move(0.f);
    if (keys[GLFW_KEY_W])
        move.z() -= 1.f;
    if (keys[GLFW_KEY_S])
        move.z() += 1.f;
    if (keys[GLFW_KEY_A])
        move.x() -= 1.f;
    if (keys[GLFW_KEY_D])
        move.x() += 1.f;
    camera.apply_move(move, cameraSpeed * t);

    if (keys[GLFW_KEY_UP])
        model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(15.0f * t));
    if (keys[GLFW_KEY_DOWN])
        model.rotare(ta::vec3(1.f, 0.f, 0.f), -ta::rad(15.0f * t));
    if (keys[GLFW_KEY_LEFT])
        model.rotare(ta::vec3(0.f, 1.f, 0.f), ta::rad(15.0f * t));
    if (keys[GLFW_KEY_RIGHT])
        model.rotare(ta::vec3(0.f, 1.f, 0.f), -ta::rad(15.0f * t));
}

std::tuple<std::vector<uint32_t>, std::vector<ta::vec3>> backface_culling(const ta::vec3& eye, std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices)
{
    std::vector<uint32_t>resi;
    constexpr size_t triangle{ 3 };

    for (auto&& tri : indices | std::views::chunk(triangle))
    {
        ta::vec3 v1 = vertices[tri[0]],
            v2 = vertices[tri[1]],
            v3 = vertices[tri[2]];

        auto v12 = v2 - v1;
        auto v13 = v3 - v1;
        auto n = ta::normalize(ta::cross(v12, v13));

        auto dir = ta::normalize(v1 - eye);

        auto dotr = ta::dot(dir, n);

        if (dotr > 0.f)
        {
            resi.insert(resi.end(), tri.begin(), tri.end());
        }
    }

    return std::make_tuple(std::move(indices), std::move(vertices));
}

std::tuple<std::vector<uint32_t>, std::vector<ta::vec3>> view_frustum_culling(std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices)
{
    std::vector<uint32_t>resi;
    std::vector<ta::vec3>resv;
    constexpr size_t triangle{ 3 };
    for (auto&& tri : indices | std::views::chunk(triangle))
    {
        ta::vec3 v1 = vertices[tri[0]],
            v2 = vertices[tri[1]],
            v3 = vertices[tri[2]];

        auto in_normal_range = [](const ta::vec3& v)
            {
                return (
                    v.x() >= -1.f && v.x() < 1.f &&
                    v.y() >= -1.f && v.y() < 1.f &&
                    v.z() >= -1.f && v.z() < 1.f);
            };

        if (in_normal_range(v1) && in_normal_range(v2) && in_normal_range(v3))
        {
            resi.insert(resi.end(), tri.begin(), tri.end());
        }
    }

    return std::make_tuple(resi, std::move(vertices));
}

void rasterization(std::tuple<int, int, int, int> image_box, ScreenBuffer& scbuffer, std::vector<uint32_t>& indices, std::vector<ta::vec3>& vertices)
{
    ta::vec3 light_pos(0.f, 300.f, 0.f);

    auto&& [boxminx, boxminy, boxmaxx, boxmaxy] = image_box;

    for (auto&& tri : indices | std::views::chunk(3))
    {
        std::array<ta::vec3, 3> vtcs{ vertices[tri[0]] ,vertices[tri[1]], vertices[tri[2]] };

        ta::vec2i bboxmin(boxmaxx - 1, boxmaxy - 1);
        ta::vec2i bboxmax(boxminx, boxminy);
        ta::vec2i clamp = bboxmin;

        for (auto&& v : vtcs)
        {
            bboxmin.x() = std::clamp(static_cast<int>(std::ceil(v.x())), 0, bboxmin.x());
            bboxmin.y() = std::clamp(static_cast<int>(std::ceil(v.y())), 0, bboxmin.y());

            bboxmax.x() = std::clamp(static_cast<int>(std::ceil(v.x())), bboxmax.x(), clamp.x());
            bboxmax.y() = std::clamp(static_cast<int>(std::ceil(v.y())), bboxmax.y(), clamp.y());
        }

        ta::vec2i p;

        for (p.x() = bboxmin.x(); p.x() <= bboxmax.x(); p.x()++)
        {
            for (p.y() = bboxmin.y(); p.y() <= bboxmax.y(); p.y()++)
            {
                auto b = ta::barycentric(vtcs[0], vtcs[1], vtcs[2], p);
                if (!b || (b->x() < 0.f) || (b->y() < 0.f) || (b->z() < 0.f))
                    continue;

                float z = 0.f;
                for (auto&& [v, bc] : std::views::zip(vtcs, *b))
                    z += v.z() * bc;

                if (scbuffer.z(p.y())[p.x()] < z)
                {
                    scbuffer.z(p.y())[p.x()] = z;
                    scbuffer[p.y()][p.x()] = ta::vec3(0.7f);
                }
            }
        }
    }
}