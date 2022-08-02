#include "SimpleEngineCore/Window.hpp"
#include "SimpleEngineCore/Log.hpp"

#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"
#include "SimpleEngineCore/Modules/UIModule.hpp"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include "glm/mat3x3.hpp"
#include "glm/trigonometric.hpp"

namespace SimpleEngine {

    
    Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height })
	{
		int resultCode = init();
	}

	

    int Window::init() {
        LOG_INFO("Creating window \"{0}\" size {1}x{2}", m_data.title, m_data.width, m_data.height);

        glfwSetErrorCallback(
            [](int error_code, const char* description) {
                LOG_CRITICAL("GLFW error: {0}", description);
            }
        );

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
            }
        );

        glfwSetWindowCloseCallback(m_pWindow, 
            [](GLFWwindow* pWindow) {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)));
                
                EventWindowClose event;
                data.eventCallbackFn(event);
            }
        );

        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y) {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)));

                EventMouseMoved event(x, y);

                data.eventCallbackFn(event);
            }
        );

        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height) {
                Renderer_OpenGL::set_view_port(width, height);
            }
        );

        glfwSetKeyCallback(m_pWindow,
            [](GLFWwindow* pWindow, int key, int scancode, int action, int modes) {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow)));
                switch (action) {
                    case (GLFW_PRESS): {
                        EventKeyPressed event(static_cast<KeyCode>(key), false);
                        data.eventCallbackFn(event);
                        break;
                    }
                    case(GLFW_RELEASE): {
                        EventKeyReleased event(static_cast<KeyCode>(key));
                        data.eventCallbackFn(event);
                        break;
                    }
                    case(GLFW_REPEAT): {
                        EventKeyPressed event(static_cast<KeyCode>(key), true);
                        data.eventCallbackFn(event);
                        break;
                    }
                }
            }
        );

        UIModule::on_window_create(m_pWindow);

        return 0;
	}

    void Window::on_update() {
        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

	void Window::shutdown() {
        UIModule::on_window_close();
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
	}

    Window::~Window() {
        shutdown();
    }
}