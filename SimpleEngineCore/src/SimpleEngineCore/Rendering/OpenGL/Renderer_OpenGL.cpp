#include "SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"
#include "SimpleEngineCore/Log.hpp"
#include "VertexArray.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace SimpleEngine {



	bool Renderer_OpenGL::init(GLFWwindow* pWindow) {
		glfwMakeContextCurrent(pWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			LOG_CRITICAL("Faled to initialize GLAD");
			return false;
		}

		LOG_INFO("OpenGl context initialized:");
		LOG_INFO("  Vendor: {0}", get_vendor_str());
		LOG_INFO("  Renderer: {0}", get_renderer_str());
		LOG_INFO("  Version: {0}", get_version_str());

		return true;
	}

	void Renderer_OpenGL::draw(const VertexArray& vertex_array) {
		vertex_array.bind();
		glDrawElements(GL_TRIANGLES, vertex_array.get_indeces_count(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer_OpenGL::set_clear_color(const float r, const float g, const float b, const float a) {
		glClearColor(r, g, b, a);
	}

	void Renderer_OpenGL::clear() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer_OpenGL::set_view_port(const unsigned int width, const unsigned int height, const unsigned int left_offset, const unsigned int bottom_offset) {
		glViewport(left_offset, bottom_offset, width, height);
	}

	const char* Renderer_OpenGL::get_vendor_str() {
		return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	}

	const char* Renderer_OpenGL::get_renderer_str() {
		return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}

	const char* Renderer_OpenGL::get_version_str() {
		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}

}