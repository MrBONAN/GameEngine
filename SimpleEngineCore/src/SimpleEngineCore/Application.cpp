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
#include "SimpleEngineCore/Input.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "glm/mat3x3.hpp"
#include "glm/trigonometric.hpp"

#include <iostream>

namespace SimpleEngine {

    std::unique_ptr<class ShaderProgram> p_shader_program;
    std::unique_ptr<class VertexArray> p_vao;

    GLfloat points_colors[]{
        // position                  color            texture
         0.0f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,    2.0f, -1.0f,
         0.0f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   -1.0f, -1.0f,
         0.0f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,    2.0f, 2.0f,
         0.0f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   -1.0f, 2.0f,
    };

    GLuint indices[]{
        0, 1, 2, 3, 2, 1
    };

    void generate_circle(unsigned char* data, 
                        const unsigned int width,
                        const unsigned int height, 
                        const unsigned int center_x, 
                        const unsigned int center_y, 
                        const unsigned int radius, 
                        const unsigned char color_r,
                        const unsigned char color_g, 
                        const unsigned char color_b) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) < radius * radius) {
                    data[(width * y + x) * 3 + 0] = color_r;
                    data[(width * y + x) * 3 + 1] = color_g;
                    data[(width * y + x) * 3 + 2] = color_b;
                }
            }
        }
    }

    void generate_smile_texture(unsigned char* data,
                                const unsigned int width,
                                const unsigned int height) {
        // background
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                data[(width * y + x) * 3 + 0] = 200;
                data[(width * y + x) * 3 + 1] = 200;
                data[(width * y + x) * 3 + 2] = 200;
            }
        }

        // face
        unsigned int min = (width > height ? height : width);
        generate_circle(data, width, height, width / 2, height / 2,  min * 0.4, 255, 255, 0);

        // smile
        generate_circle(data, width, height, width / 2, height * 0.4,  min * 0.2, 0, 0, 0);
        generate_circle(data, width, height, width / 2, height * 0.45, min * 0.2, 255, 255, 0);

        // eyes
        generate_circle(data, width, height, width * 0.35, height * 0.6, min * 0.07, 255, 0, 255);
        generate_circle(data, width, height, width * 0.65, height * 0.6, min * 0.07, 0, 0, 255);

    }

    void generate_squares_texture(unsigned char* data,
        const unsigned int width,
        const unsigned int height) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                char color = 0;
                if (x < width / 2 != y < height / 2) color = 255;
                data[(width * y + x) * 3 + 0] = color;
                data[(width * y + x) * 3 + 1] = color;
                data[(width * y + x) * 3 + 2] = color;
            }
        }
    }

    float scale[] = { 1.0f, 1.0f, 1.0f };
    float rotate = 0.f;
    float translate[] = { 0.0f, 0.0f , 0.f };
    float m_background_color[4]{ .33f, .33f, .33f, 0.f };

    const char* vertex_shader =
        R"(
        #version 460 core
        layout (location = 0) in vec3 vertex_position;
        layout (location = 1) in vec3 vertex_color;
        layout (location = 2) in vec2 texture_coord;
        uniform mat4 model_matrix;
        uniform mat4 view_projection_matrix;
        uniform int current_frame;        

        out vec3 color;
        out vec2 text_coord_smile;
        out vec2 text_coord_squares;

        void main() {
        	color = vertex_color;
        	gl_Position = view_projection_matrix * model_matrix * vec4(vertex_position, 1.0);
            text_coord_smile = texture_coord;
            text_coord_squares = texture_coord + vec2(current_frame / 1000.f, current_frame / 1000.f);
        }
        )";

    const char* fragment_shader =
        R"(
        #version 460 core

        in vec3 color;
        in vec2 text_coord_smile;
        in vec2 text_coord_squares;
        out vec4 frag_color;
        layout(binding = 0) uniform sampler2D smile_Texture;
        layout(binding = 1) uniform sampler2D squares_Texture;        

        void main() {
        	//frag_color = vec4(color, 1.0);
            frag_color = texture(smile_Texture, text_coord_smile) * texture(squares_Texture, text_coord_squares);
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

        m_event_dispatcher.add_event_listener<EventKeyPressed>(
            [&](EventKeyPressed& event) {
                if(event.key_code <= KeyCode::KEY_Z) LOG_INFO("[{0}] code: {1}", (event.repeated? "KeyRepeated" : "KeyPressed"), char(event.key_code));
                if (!event.repeated) Input::PressKey(event.key_code);
            });

        m_event_dispatcher.add_event_listener<EventKeyReleased>(
            [&](EventKeyReleased& event) {
                if (event.key_code <= KeyCode::KEY_Z) LOG_INFO("[KeyReleased] code: {0}", char(event.key_code));
                Input::ReleaseKey(event.key_code);
            });

        m_event_dispatcher.add_event_listener<EventMouseButtonPressed>(
            [&](EventMouseButtonPressed& event) {
                LOG_INFO("[MouseButtonPressed] code: {0} at {1}-X {2}-Y", static_cast<size_t>(event.mouse_button), event.x, event.y);
                Input::PressMouseButton(event.mouse_button);
                on_mouse_button_event(event.mouse_button, event.x, event.y, true);
            });

        m_event_dispatcher.add_event_listener<EventMouseButtonReleased>(
            [&](EventMouseButtonReleased& event) {
                LOG_INFO("[MouseButtonReleased] code: {0} at {1}-X {2}-Y", static_cast<size_t>(event.mouse_button), event.x, event.y);
                Input::ReleaseMouseButton(event.mouse_button);
                on_mouse_button_event(event.mouse_button, event.x, event.y, false);
            });

		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				//LOG_INFO("[MouseMoved] Mouse moved to {0}-X {1}-Y", event.x, event.y);
			});
        const unsigned int width = 1000;
        const unsigned int height = 1000;
        const unsigned int channels = 3;

        unsigned char* data = new unsigned char[width * height * channels];

        GLuint smileTexture;
        glCreateTextures(GL_TEXTURE_2D, 1, &smileTexture);
        glTextureStorage2D(smileTexture, 1, GL_RGB8, width, height);
        generate_smile_texture(data, width, height);
        glTextureSubImage2D(smileTexture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTextureParameteri(smileTexture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(smileTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(smileTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(smileTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTextureUnit(0, smileTexture);

        GLuint SquaresTexture;
        glCreateTextures(GL_TEXTURE_2D, 1, &SquaresTexture);
        glTextureStorage2D(SquaresTexture, 1, GL_RGB8, width, height);
        generate_squares_texture(data, width, height);
        glTextureSubImage2D(SquaresTexture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTextureParameteri(SquaresTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(SquaresTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(SquaresTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(SquaresTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTextureUnit(1, SquaresTexture);

        delete[] data;

        //-----------------------------------//
        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->is_compiled()) return false;


        p_vao = std::make_unique<VertexArray>();

        BufferLayout buffer_layout{
            ShaderDataType::Float3,
            ShaderDataType::Float3,
            ShaderDataType::Float2
        };

        VertexBuffer vbo(points_colors, sizeof(points_colors), buffer_layout);
        IndexBuffer indexBuffer(indices, sizeof(indices) / sizeof(GLuint));

        p_vao->add_vertex_buffer(vbo);
        p_vao->set_index_buffer(indexBuffer);

        VertexArray::unbind();
        //-----------------------------------//

        int frame = 0;

		while (!m_bCloseWindow) {

            //-----------------------------------//
            Renderer_OpenGL::set_clear_color(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
            Renderer_OpenGL::clear();

            glm::mat4 scale_matrix(scale[0], 0, 0, 0,
                0, scale[1], 0, 0,
                0, 0, scale[2], 0,
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

            camera.set_projection_mode(is_perspective_mode ? Camera::ProjectionMode::Perspective : Camera::ProjectionMode::Orthographic);

            p_shader_program->bind();
            p_shader_program->set_matrix4("model_matrix", model_matrix);
            p_shader_program->set_matrix4("view_projection_matrix", camera.get_projection_matrix() * camera.get_view_matrix());
            p_shader_program->set_int("current_frame", frame++);

            Renderer_OpenGL::draw(*p_vao);

            ShaderProgram::unbind();

            //-----------------------------------//
            UIModule::on_ui_draw_begin();
            bool show = false;
            UIModule::ShowExampleAppDockSpace(&show);
            ImGui::ShowDemoWindow();
            ImGui::Begin("Background color window");
            ImGui::ColorEdit4("background color", m_background_color);
            ImGui::SliderFloat3("scale", scale, 0, 2);
            ImGui::SliderFloat("rotate", &rotate, 0, 360);
            ImGui::SliderFloat3("translate", translate, -1, 1);
            ImGui::End();
            //-----------------------------------//

            on_ui_draw();

            UIModule::on_ui_draw_end();

			m_pWindow->on_update();
			on_update();
		}

        glDeleteTextures(1, &smileTexture);
        glDeleteTextures(1, &SquaresTexture);
		m_pWindow = nullptr;

		return 0;
	}
    glm::vec2 Application::get_current_cursor_position()
    {
        return std::move(m_pWindow->get_current_cursor_position());
    }
}