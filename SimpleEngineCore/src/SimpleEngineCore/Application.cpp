#include "SimpleEngineCore/Application.hpp"
#include "SimpleEngineCore/Log.hpp" 
#include "SimpleEngineCore/Window.hpp"

#include "SimpleEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include "SimpleEngineCore/Camera.hpp"
#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"
#include "SimpleEngineCore/Modules/UIModule.hpp"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"

#include "glm/mat3x3.hpp"
#include "glm/trigonometric.hpp"

#include <iostream>

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

    float scale[] = { 1.0f, 1.0f };
    float rotate = 0.f;
    float translate[] = { 0.0f, 0.0f , 0.f };
    float m_background_color[4]{ .33f, .33f, .33f, 0.f };

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


	Application::Application() {
        LOG_INFO("Starting Application");
	}
	Application::~Application() {
		LOG_INFO("Closing Application");
	}

	int Application::start(unsigned __int32 window_width, unsigned __int32 window_height, const char* title) {
		m_pWindow = std::make_unique<Window>(title, window_width, window_height);

		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			});


		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[](EventWindowResize& event) {
				LOG_INFO("[WindowResized] Change size to {0}x{1}", event.width, event.height);
			});

		m_event_dispatcher.add_event_listener<EventWindowClose>(
			[&](EventWindowClose& event) {
				LOG_INFO("[WindowClose] Window closed");
				m_bCloseWindow = true;
			});

		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				//LOG_INFO("[MouseMoved] Mouse moved to {0}-X {1}-Y", event.x, event.y);
			});



        //-----------------------------------//
        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled()) return false;


        p_vao = std::make_unique<VertexArray>();

        BufferLayout buffer_layout{
            ShaderDataType::Float2,
            ShaderDataType::Float3
        };

        VertexBuffer vbo(points_colors, sizeof(points_colors), buffer_layout);
        IndexBuffer indexBuffer(indices, sizeof(indices) / sizeof(GLuint));

        p_vao->add_vertex_buffer(vbo);
        p_vao->set_index_buffer(indexBuffer);

        VertexArray::unbind();
        //-----------------------------------//


		while (!m_bCloseWindow) {

            //-----------------------------------//
            Renderer_OpenGL::set_clear_color(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
            Renderer_OpenGL::clear();

            glm::mat4 scale_matrix(scale[0], 0, 0, 0,
                0, scale[1], 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 1);

            float rotate_in_radians = glm::radians(rotate);
            glm::mat4 rotate_matrix(cos(rotate_in_radians), sin(rotate_in_radians), 0, 0,
                -sin(rotate_in_radians), cos(rotate_in_radians), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);

            glm::mat4 translate_matrix(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
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

            //-----------------------------------//
            UIModule::on_ui_draw_begin();
            bool show = false;
            UIModule::ShowExampleAppDockSpace(&show);
            ImGui::ShowDemoWindow();
            ImGui::Begin("Background color window");
            ImGui::ColorEdit4("background color", m_background_color);
            ImGui::SliderFloat2("scale", scale, 0, 2);
            ImGui::SliderFloat("rotate", &rotate, 0, 360);
            ImGui::SliderFloat3("translate", translate, -1, 1);
            ImGui::End();
            //-----------------------------------//

            on_ui_draw();

            UIModule::on_ui_draw_end();

			m_pWindow->on_update();
			on_update();
		}

		m_pWindow = nullptr;

		return 0;
	}
}