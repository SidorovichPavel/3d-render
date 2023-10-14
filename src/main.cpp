#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

#include <tinyalgebralib/math/math.hpp>
#include <glfwextlib/Window.hpp>

#include "Model/Model.hpp"

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 color;
    layout (location = 2) in vec2 tex_vertex;

    out vec3 fcolor;
    out vec2 ftex_vertex;

    void main()
    {
        gl_Position = vec4(position.x, position.y, position.z, 1.0);
        fcolor = color;
        ftex_vertex = tex_vertex;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 fcolor;
    in vec2 ftex_vertex;

    out vec4 FragColor;
    
    uniform sampler2D ftexture;

    void main()
    {
        FragColor = texture(ftexture, ftex_vertex);
    }
)";


struct Camera
{
    ta::vec3 pos, dir, up;
};

void do_movement(const glfwext::Window& window, Camera& camera, Model& model, float yaw, float pitch, float ms);
std::vector<ta::vec2i> apply_viewport(const ta::mat4& viewport, const std::vector<ta::vec3>& transformed_vertices);


int main()
{
    // Инициализация GLFW
    if (!glfwInit())
    {
        std::cerr << "Ошибка инициализации GLFW" << std::endl;
        return -1;
    }

    ta::vec2i screen_size{ 1280, 720 };

    std::unique_ptr<glfwext::Window> window;
    try
    {
        window = std::make_unique<glfwext::Window>("GLFW Window", screen_size.x(), screen_size.y());
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    window->make_current();

    window->framebuffer_resize += [](glfwext::Window* window, int width, int height) {
        glViewport(0, 0, width, height);
        };

    bool mouse_input = false;

    window->key_press += [&mouse_input](glfwext::Window* window, int key, int scancode, int mode) {
        if (key == GLFW_KEY_TAB)
        {
            mouse_input = !mouse_input;
            if (mouse_input)
                window->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else
                window->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        };
    window->key_press += [](glfwext::Window* window, int key, int scancode, int mode) {
        if (key == GLFW_KEY_ESCAPE)
            window->set_should_close(true);
        };

    float lastX, lastY;
    float pitch;
    float yaw;

    lastX = 400;
    lastY = 300;

    pitch = 0.0f;
    yaw = 180.0f;

    window->cursor_move += [&](glfwext::Window*, float xpos, float ypos) {
        if (!mouse_input)
            return;

        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY; // Обратный порядок вычитания потому что оконные Y-координаты возрастают с верху вниз
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        };

    // Инициализация GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Ошибка инициализации GLEW" << std::endl;
        return -1;
    }

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
    auto [width, height] = window->size();
    std::vector<ta::vec3> image(width * height);
    for (auto& c : image)
        c = bgd_color;
    // Load and create a texture 
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, image.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

    Camera camera;
    camera.pos = ta::vec3(350.f, 0.f, 0.f);
    camera.dir = ta::vec3(-1.f, 0.f, 0.f);
    camera.up = ta::vec3(0.f, 1.f, 0.f);

    std::string_view filename("/mnt/sata0/Workshop/3d-render/resources/models/hyperion.stl");
    Model model;
    model.load_from_file(filename);
    model.scale(ta::vec3(2.f));
    model.translate(ta::vec3(0.f, 0.f, 100.f));
    model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(90.f));

    auto time = std::chrono::steady_clock::now();

    for (; !window->should_close();)
    {
        auto rend_begin = std::chrono::steady_clock::now();

        glfwPollEvents();
        auto tp = std::chrono::steady_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(time - tp).count() / 1000.f;
        time = tp;

        do_movement(*window, camera, model, yaw, pitch, dt);

        auto projection = ta::perspective(ta::rad(90.f), window->ratio(), .1f, 500.f);
        auto view = ta::look_at(camera.pos, camera.pos + camera.dir, camera.up);
        auto [width, height] = window->size();
        auto tdata = model.transform(view, projection);

        auto viewport = ta::viewport(0, 0, width, height);

        auto pixels = apply_viewport(viewport, tdata);

        // Bind Texture
        if (image.size() != width * height)
            image.resize(width * height);

        for (auto& c : image)
            c = bgd_color * 2;

        for (auto pxl : pixels)
        {
            if ((pxl.x() >= width || pxl.y() >= height) &&
                (pxl.x() > 0 || pxl.y() < 0))
                continue;

            image[pxl.y() * width + pxl.x()] = ta::vec3(1.f);
        }

        std::apply(glClearColor, bgd_tuple_color);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, image.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        // Activate shader
        glUseProgram(shaderProgram);

        // Draw container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        window->swap_buffers();
        std::cout << 1000.f / std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - rend_begin).count() << std::endl;
    }

    // Освобождение ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(shaderProgram);

    // Завершение работы GLFW
    glfwTerminate();
    return 0;
}

void do_movement(const glfwext::Window& window, Camera& camera, Model& model, float yaw, float pitch, float ms)
{
    if (ms == INFINITY)
        ms = 0.0001f;

    decltype(auto) keys = window.keys_state();

    camera.dir.x() = std::cos(ta::rad(pitch)) * std::cos(ta::rad(yaw));
    camera.dir.y() = std::sin(ta::rad(pitch));
    camera.dir.z() = std::cos(ta::rad(pitch)) * std::sin(ta::rad(yaw));
    camera.dir = ta::normalize(camera.dir);

    float cameraSpeed = 200.f;

    if (keys[GLFW_KEY_W])
        camera.pos += cameraSpeed * -camera.dir * ms;
    if (keys[GLFW_KEY_S])
        camera.pos -= cameraSpeed * -camera.dir * ms;
    if (keys[GLFW_KEY_A])
        camera.pos -= ta::normalize(ta::cross(camera.up, camera.dir)) * cameraSpeed * ms;
    if (keys[GLFW_KEY_D])
        camera.pos += ta::normalize(ta::cross(camera.up, camera.dir)) * cameraSpeed * ms;
    if (keys[GLFW_KEY_UP])
        model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(15.0f * ms));
    if (keys[GLFW_KEY_DOWN])
        model.rotare(ta::vec3(1.f, 0.f, 0.f), -ta::rad(15.0f * ms));
    if (keys[GLFW_KEY_LEFT])
        model.rotare(ta::vec3(0.f, 1.f, 0.f), ta::rad(15.0f * ms));
    if (keys[GLFW_KEY_RIGHT])
        model.rotare(ta::vec3(0.f, 1.f, 0.f), -ta::rad(15.0f * ms));
}

std::vector<ta::vec2i> apply_viewport(const ta::mat4& viewport, const std::vector<ta::vec3>& transformed_vertices)
{
    std::vector<ta::vec2i> result;

    for (auto& v : transformed_vertices)
    {
        if (std::abs(v.x()) - 1.f > std::numeric_limits<float>::epsilon() ||
            std::abs(v.y()) - 1.f > std::numeric_limits<float>::epsilon() ||
            std::abs(v.z()) - 1.f > std::numeric_limits<float>::epsilon())
            continue;

        auto vtx = viewport * ta::vec4(v, 1.f);
        result.emplace_back(static_cast<int>(vtx.x()), static_cast<int>(vtx.y()));
    }

    return result;
}