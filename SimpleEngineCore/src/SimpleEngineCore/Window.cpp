#include "SimpleEngineCore/Window.hpp"
#include "SimpleEngineCore/Log.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

namespace SimpleEngine {

    static bool s_GLFW_initialized = false;

    std::unique_ptr<class ShaderProgram> p_shader_program;
    GLuint vao;

    GLfloat points[]{
         0.0f,	0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f
    };

    GLfloat colors[]{
         1.0f,	0.0f, 0.0f,
         0.0f,	1.0f, 0.0f,
         0.0f,	0.0f, 1.0f
    };

    const char* vertex_shader =
        "#version 460 core\n"
        "layout (location = 0) in vec2 vertex_position;"
        "layout (location = 1) in vec3 vertex_color;"
        "out vec3 color;"
        "void main() {"
        "	color = vertex_color;"
        "	gl_Position = vec4(vertex_position, 0.0, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 460 core\n"
        "in vec3 color;"
        "out vec4 frag_color;"
        "void main() {"
        "	frag_color = vec4(color, 1.0);"
        "}";

    Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height })
	{
		int resultCode = init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
	}

	void Window::on_update() {

        glClearColor(m_background_color[0], m_background_color[1],  m_background_color[2], m_background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        p_shader_program->bind();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        ImGui::Begin("Background color window");
        ImGui::ColorEdit4("background color", m_background_color);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
	}

	int Window::init() {
        LOG_INFO("Creating window \"{0}\" size {1}x{2}", m_data.title, m_data.width, m_data.height);
        
        if (!s_GLFW_initialized) {
            if (!glfwInit()) {
                LOG_CRITICAL("Can't initialize GLFW");
                return -1;
            }
            s_GLFW_initialized = true;
        }

        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (!m_pWindow)
        {
            LOG_CRITICAL("Can't create window {0}", m_data.title);
            glfwTerminate();
            return -2;
        }

        glfwMakeContextCurrent(m_pWindow);
        
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            LOG_CRITICAL("Faled to initialize GLAD");
            return -3;
        }

        glfwSetWindowUserPointer(m_pWindow, &m_data);

        glfwSetWindowSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height) {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)));
                data.width = width;
                data.height = height;

                EventWindowResize event(width, height);
                data.eventCallbackFn(event);
            });

        glfwSetWindowCloseCallback(m_pWindow, 
            [](GLFWwindow* pWindow) {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)));
                
                EventWindowClose event;
                data.eventCallbackFn(event);
            });

        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y) {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)));

                EventMouseMoved event(x, y);

                data.eventCallbackFn(event);
            });

        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height) {
                glViewport(0, 0, width, height);
            });
        //auto vs = std::move(downloadShaderSrc("src\\shaders\\vertex_shader.shd"));
        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled()) return false;

        GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        GLuint colors_vbo = 0;
        glGenBuffers(1, &colors_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);


        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindVertexArray(0);

        return 0;
	}

	void Window::shutdown() {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
	}

    Window::~Window() {
        shutdown();
    }
}