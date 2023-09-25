#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>

#include <tinyalgebralib/math/math.hpp>

int main()
{

    // Инициализация GLFW
    if (!glfwInit())
    {
        std::cerr << "Ошибка инициализации GLFW" << std::endl;
        return -1;
    }

    ta::vec2i screen_size{800, 600};

    // Создание окна GLFW
    GLFWwindow *window = glfwCreateWindow(screen_size.x(), screen_size.y(), "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        std::cerr << "Ошибка создания окна GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Активация окна GLFW
    glfwMakeContextCurrent(window);

    // Инициализация GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Ошибка инициализации GLEW" << std::endl;
        return -1;
    }

    // Точки экрана

    // Создание и компиляция вершинного шейдера
    const char *vertexShaderSource = R"(#version 330 core
        layout(location = 0) in vec3 aPos;
        void main() {
           gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
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
        {
            poss.emplace_back(
                (2.f * static_cast<float>(j) / screen_size.x()) - 1.f, 
                (2.f * static_cast<float>(i) / screen_size.y()) - 1.f);
        }

    
    // Создание буфера вершин и привязка данных
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(poss)::value_type) * poss.size(), poss.data(), GL_STATIC_DRAW);

    // Указание атрибутов вершинного шейдера
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    // Рендеринг
    for (; !glfwWindowShouldClose(window);)
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, poss.size());

        glfwSwapBuffers(window);
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