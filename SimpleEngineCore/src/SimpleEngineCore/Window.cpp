#include "SimpleEngineCore/Window.hpp"
#include "SimpleEngineCore/Log.hpp"

#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include "SimpleEngineCore/Camera.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include "glm/mat3x3.hpp"
#include "glm/trigonometric.hpp"

namespace SimpleEngine {

    std::unique_ptr<class ShaderProgram> p_shader_program;
    std::unique_ptr<class VertexArray> p_vao;

    GLfloat points_colors[]{
        // position                  color
         -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,
          0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
         -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
          0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
    };

    GLuint indices[]{
        0, 1, 2, 3, 2, 1
    };
    
    float scale[] = {1.0f, 1.0f};
    float rotate = 0.f;
    float translate[] = { 0.0f, 0.0f , 0.f};

    float camera_position[] = { 0.0f, 0.0f, 1.0f };
    float camera_rotation[] = { 0.0f, 0.0f, 0.0f };
    bool is_perspective_mode = false;
    Camera camera;

    const char* vertex_shader =
        R"(
        #version 460 core
        layout (location = 0) in vec2 vertex_position;
        layout (location = 1) in vec3 vertex_color;
        uniform mat4 model_matrix;
        uniform mat4 view_projection_matrix;
        out vec3 color;
        void main() {
        	color = vertex_color;
        	gl_Position = view_projection_matrix * model_matrix * vec4(vertex_position, 0.0, 1.0);
        }
        )";

    const char* fragment_shader =
        R"(
        #version 460 core
        in vec3 color;
        out vec4 frag_color;
        void main() {
        	frag_color = vec4(color, 1.0);
        }
        )";

    Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height })
	{
		int resultCode = init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
	}

	

    int Window::init() {
        LOG_INFO("Creating window \"{0}\" size {1}x{2}", m_data.title, m_data.width, m_data.height);

        glfwSetErrorCallback([](int error_code, const char* description) {
            LOG_CRITICAL("GLFW error: {0}", description);
            });

        if (!glfwInit()) {
            LOG_CRITICAL("Can't initialize GLFW");
            return -1;
        }

        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (!m_pWindow)
        {
            LOG_CRITICAL("Can't create window {0}", m_data.title);
            return -2;
        }

        if (!Renderer_OpenGL::init(m_pWindow)) {
            LOG_INFO("Failed initialize OpenGL renderer");
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
                Renderer_OpenGL::set_view_port(width, height);
            });

        //auto vs = std::move(downloadShaderSrc("src\\shaders\\vertex_shader.shd"));

        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled()) return false;


        p_vao = std::make_unique<VertexArray>();

        BufferLayout buffer_layout{
            ShaderDataType::Float2,
            ShaderDataType::Float3
        };

        VertexBuffer vbo (points_colors, sizeof(points_colors), buffer_layout);
        IndexBuffer indexBuffer(indices, sizeof(indices) / sizeof(GLuint));

        p_vao->add_vertex_buffer(vbo);
        p_vao->set_index_buffer(indexBuffer);

        VertexArray::unbind();

        return 0;
	}

    void Window::on_update() {

        Renderer_OpenGL::set_clear_color(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
        Renderer_OpenGL::clear();


        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(get_width());
        io.DisplaySize.y = static_cast<float>(get_height());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Background color window");
        ImGui::ColorEdit4("background color", m_background_color);
        ImGui::SliderFloat2("scale", scale, 0, 2);
        ImGui::SliderFloat("rotate", &rotate, 0, 360);
        ImGui::SliderFloat3("translate", translate, -1, 1);

        ImGui::SliderFloat3("camera position", camera_position, -1, 1);
        ImGui::SliderFloat2("camera rotation", camera_rotation, 0, 360);
        ImGui::Checkbox("Perspective camera", &is_perspective_mode);

        glm::mat4 scale_matrix( scale[0], 0, 0, 0,
                                0, scale[1], 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 1);

        float rotate_in_radians = glm::radians(rotate);
        glm::mat4 rotate_matrix(cos(rotate_in_radians), sin(rotate_in_radians), 0, 0,
                               -sin(rotate_in_radians), cos(rotate_in_radians), 0, 0,
                                0,                      0,                      1, 0,
                                0,                      0,                      0, 1);

        glm::mat4 translate_matrix(1,            0,            0, 0,
                                   0,            1,            0, 0,
                                   0,            0,            1, 0,
                                   translate[0], translate[1], translate[2], 1);


        glm::mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;


        camera.set_position_rotation(glm::vec3(camera_position[0], camera_position[1], camera_position[2]),
                                     glm::vec3(camera_rotation[0], camera_rotation[1], camera_rotation[2]));
        camera.set_projection_mode(is_perspective_mode ? Camera::ProjectionMode::Perspective : Camera::ProjectionMode::Orthographic);

        p_shader_program->bind();
        p_shader_program->setMatrix4("model_matrix", model_matrix);
        p_shader_program->setMatrix4("view_projection_matrix", camera.get_projection_matrix() * camera.get_view_matrix());

        Renderer_OpenGL::draw(*p_vao);

        ShaderProgram::unbind();

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

	void Window::shutdown() {

        if (ImGui::GetCurrentContext()) ImGui::DestroyContext();

        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
	}

    Window::~Window() {
        shutdown();
    }
}