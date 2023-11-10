#include "App.hpp"

App::App(int argc, char* args[])
    :
    camera(ta::vec3(390.f, 0.f, 0.f), ta::vec3(0.f, 0.f, 0.f), ta::vec3(0.f, 1.f, 0.f)),
    pool(12u),
    engine_(16)
{
    ta::vec2i screen_size{ 1280, 720 };
    try
    {
        glfwext::init(); // init GLFW3
        window = std::make_unique<glfwext::Window>("GLFW Window", screen_size.x(), screen_size.y());
        window->make_current(); // make current gl context
        glewext::init(); // init glew
        engine_.init(1280, 720);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    window->framebuffer_resize += [this](glfwext::Window* window, int width, int height) {
        engine_.resize(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
        glViewport(0, 0, width, height);
        };

    window->key_press += [this](glfwext::Window* window, int key, int scancode, int mode) {
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

    window->scroll += [this](glfwext::Window* window, float xoffset, float yoffset)
        {
            if (!mouse_input)
                return;

            fovy += yoffset;
            if (fovy > 120.f)
                fovy = 120.f;
            if (fovy < 60.f)
                fovy = 60.f;
        };

    window->cursor_move += [this](glfwext::Window*, float xpos, float ypos) {
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

    std::string_view filename("/mnt/sata0/Workshop/3d-render/resources/models/hyperion.stl");
    model.load_from_file(filename);
    model.rotare(ta::vec3(1.f, 0.f, 0.f), ta::rad(90.f));
}

App::~App()
{
    glfwTerminate();
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
                    scbuffer[p.y()][p.x()] = ta::vec3(0.9f);
                }
            }
        }
    }
}

void App::run()
{
    if (!window)
        return;

    auto time = std::chrono::steady_clock::now();

    for (; !window->should_close();)
    {
        auto rend_begin = std::chrono::steady_clock::now();

        glfwPollEvents();
        auto tp = std::chrono::steady_clock::now();
        auto frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(time - tp).count() / 1000.f;
        time = tp;

        movement(frame_time);

        auto projection = ta::perspective(ta::rad(fovy), window->ratio(), .1f, 500.f);
        auto view = camera.get_view();

        // draw to opengl context
        engine_.display();

        //draw model
        //(*engine_)(model);

        // reset engine state
        engine_.reset();

        window->swap_buffers();
        std::cout << /* 1000.f / */ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - rend_begin).count() << std::endl;
    }
}

void App::movement(float t) noexcept
{
    if (t == INFINITY)
        t = 0.0001f;

    decltype(auto) keys = window->keys_state();

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

