#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <type_traits>
#include <map>

namespace glfw{

class Window
{
public:
    Window(const std::string& title, int width, int height) noexcept;
    ~Window();


    void make_current() const noexcept;
    void swap_buffers() const noexcept;
    bool should_close() const noexcept;

private:
    static void register_window(GLFWwindow* glfw_wdn, Window *wnd);
    
    static void static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

public:

    void key_callback(int key, int scancode, int action, int mode);

private:
    GLFWwindow* window_;
    int64_t width_, height_;
    std::string title_;

    static std::map<GLFWwindow*, Window*> windows_map;
};


}