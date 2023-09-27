#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <memory>

#include <tinyalgebralib/math/math.hpp>

#include "Model/Model.hpp"
#include "Window/window.hpp"

int main()
{
    std::string_view filename("/mnt/sata0/Workshop/3d-render/resources/models/hyperion.stl");

    Model model;
    model.load_from_file(filename);

    // Инициализация GLFW
    if (!glfwInit())
    {
        std::cerr << "Ошибка инициализации GLFW" << std::endl;
        return -1;
    }

    ta::vec2i screen_size{800, 600};

    std::unique_ptr<glfw::Window> window;
    try
    {
        window = std::make_unique<glfw::Window>("GLFW Window", screen_size.x(), screen_size.y());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    window->make_current();

    // Инициализация GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Ошибка инициализации GLEW" << std::endl;
        return -1;
    }

    // Создание и компиляция вершинного шейдера
    const char *vertexShaderSource = R"(#version 330 core
        layout(location = 0) in vec3 aPos;
        void main() {
           gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        })";

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Создание и компиляция фрагментного шейдера
    const char *fragmentShaderSource = R"(#version 330 core
        out vec4 FragColor;
        void main() {
           FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        })";

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Создание и связывание шейдерной программы
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    std::vector<ta::vec2f> poss;
    for (int i = 0; i < screen_size.y(); i++)
        for (int j = 0; j < screen_size.x(); j++)
            poss.emplace_back(
                (2.f * static_cast<float>(j) / screen_size.x()) - 1.f,
                (2.f * static_cast<float>(i) / screen_size.y()) - 1.f);

    // Создание буфера вершин и привязка данных
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    auto view = ta::look_at(
        ta::vec3(0.f, 0.f, 200.f),
        ta::vec3(0.f, 0.f, 0.f),
        ta::vec3(0.f, 1.f, 0.f));

    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices_sizeof(), model.indices(), GL_STATIC_DRAW);

    // Рендеринг
    for (; !window->should_close();)
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        auto projection = ta::perspective(ta::rad(120.f), window->get_ratio(), 1.f, 400.f);

        auto data = model.transform(view, projection);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
        glBufferData(GL_ARRAY_BUFFER, data.size() * 3 * sizeof(float), data.data(), GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLES, 0, data.size());
        // glDrawElements(GL_TRIANGLES, model.indices_count(), GL_UNSIGNED_INT, nullptr);

        window->swap_buffers();
        glfwPollEvents();
    }

    // Освобождение ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Завершение работы GLFW
    glfwTerminate();
    return 0;
}