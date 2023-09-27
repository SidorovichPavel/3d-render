#include <stdexcept>

#include "window.hpp"

namespace glfw
{

    std::map<GLFWwindow *, Window *> Window::windows_map;

    Window::Window(const std::string &title, int width, int height) noexcept
        : title_(title),
          width_(width),
          height_(height)
    {
        window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
        if (!window_)
            throw std::runtime_error("glfw::CreateWindow failed");

        register_window(window_, this);
        glfwSetKeyCallback(window_, static_key_callback);
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

    bool Window::should_close() const noexcept
    {
        return glfwWindowShouldClose(window_);
    }

    void Window::register_window(GLFWwindow *glfw_wdn, Window *wnd)
    {
        windows_map[glfw_wdn] = wnd;
    }

    void Window::static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
    {
        windows_map[window]->key_callback(key, scancode, action, mode);
    }

    void Window::key_callback(int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window_, GL_TRUE);
    }
}