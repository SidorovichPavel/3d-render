#include <stdexcept>
#include <algorithm>

#include "window.hpp"

namespace glfw
{
    Window::Window(const std::string &title, int width, int height) noexcept
        : title_(title),
          width_(width),
          height_(height)
    {
        window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
        if (!window_)
            throw std::runtime_error("glfw::CreateWindow failed");

        glfwSetWindowUserPointer(window_, this);

        glfwSetKeyCallback(window_, static_key_callback);
        glfwSetCursorPosCallback(window_, static_cursor_pos_callback);
        glfwSetFramebufferSizeCallback(window_, static_framebuffer_size_callback);

        std::ranges::fill(keys_, false);

        lastX = 400;
        lastY = 300;

        pitch = 0.0f;
        yaw = -90.0f;

        //glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    Window::~Window()
    {
    }

    void Window::make_current() const noexcept
    {
        glfwMakeContextCurrent(window_);
    }

    bool Window::should_close() const noexcept
    {
        return glfwWindowShouldClose(window_);
    }

    void Window::swap_buffers() const noexcept
    {
        glfwSwapBuffers(window_);
    }

    float Window::get_ratio() const noexcept
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    const std::array<bool, 1024> &Window::get_keys_state() const noexcept
    {
        return keys_;
    }

    std::pair<float, float> Window::get_angles() const noexcept
    {
        return std::pair<float, float>(yaw, pitch);
    }

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* STATIC MEMBERS */
    /*--------------------------------------------------------------------------------------------------------------------*/

    void Window::static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
    {
        auto obj_ptr = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (obj_ptr)
            obj_ptr->key_callback(key, scancode, action, mode);
    }

    void Window::static_framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        auto obj_ptr = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (obj_ptr)
            obj_ptr->framebuffer_size_callback(width, height);
    }

    void Window::static_cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
    {
        auto obj_ptr = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (obj_ptr)
            obj_ptr->cursor_pos_callback(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* STATIC MEMBERS END*/
    /*--------------------------------------------------------------------------------------------------------------------*/

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* PROTECTED MEMBERS*/
    /*--------------------------------------------------------------------------------------------------------------------*/

    void Window::key_callback(int key, int scancode, int action, int mode)
    {
        if (action == GLFW_PRESS)
            keys_[key] = true;
        else if (action == GLFW_RELEASE)
            keys_[key] = false;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window_, GL_TRUE);
    }
    void Window::framebuffer_size_callback(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Window::cursor_pos_callback(float xpos, float ypos)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Обратный порядок вычитания потому что оконные Y-координаты возрастают с верху вниз
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
    }

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* PROTECTED MEMBERS END*/
    /*--------------------------------------------------------------------------------------------------------------------*/

}