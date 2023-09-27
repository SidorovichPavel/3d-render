#include <stdexcept>

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
        glfwSetFramebufferSizeCallback(window_, static_framebuffer_size_callback);
    }

    Window::~Window()
    {
    }

    void Window::make_current() const noexcept
    {
        glfwMakeContextCurrent(window_);
    }

    void Window::swap_buffers() const noexcept
    {
        glfwSwapBuffers(window_);
    }

    float Window::get_ratio() const noexcept
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    bool Window::should_close() const noexcept
    {
        return glfwWindowShouldClose(window_);
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

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* STATIC MEMBERS END*/
    /*--------------------------------------------------------------------------------------------------------------------*/

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* PROTECTED MEMBERS*/
    /*--------------------------------------------------------------------------------------------------------------------*/

    void Window::key_callback(int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window_, GL_TRUE);
    }
    void Window::framebuffer_size_callback(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    /*--------------------------------------------------------------------------------------------------------------------*/
    /* PROTECTED MEMBERS END*/
    /*--------------------------------------------------------------------------------------------------------------------*/

}