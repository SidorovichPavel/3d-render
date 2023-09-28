#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <type_traits>
#include <array>

namespace glfw{

class Window
{
public:
    Window(const std::string& title, int width, int height) noexcept;
    ~Window();


    void make_current() const noexcept;
    bool should_close() const noexcept;
    void swap_buffers() const noexcept;

    float get_ratio() const noexcept;
    const std::array<bool, 1024>& get_keys_state() const noexcept;
    std::pair<float, float> get_angles() const noexcept;

private:    
    static void static_window_size_callback(GLFWwindow *window, int width, int height);
    static void static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void static_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void static_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

protected:
    void key_callback(int key, int scancode, int action, int mode);
    void framebuffer_size_callback(int width, int height);
    void cursor_pos_callback(float xpos, float ypos);
private:
    GLFWwindow* window_;
    int width_, height_;
    std::string title_;
    std::array<bool, 1024> keys_;

    float lastX, lastY;
    float pitch;
    float yaw;
};


}