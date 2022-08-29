#include "ShaderProgram.hpp"
#include "SimpleEngineCore/Log.hpp"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>

namespace SimpleEngine {
	bool createShader(const char* source, GLuint shader_type, GLuint& shader_id) {
		shader_id = glCreateShader(shader_type);
		glShaderSource(shader_id, 1, &source, nullptr);
		glCompileShader(shader_id);

		GLint success;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			char info_log[1024];
			glGetShaderInfoLog(shader_id, 1024, nullptr, info_log);
			LOG_CRITICAL("SHADER COMPILATION ERROR:\n{}", info_log);
			return false;
		}
		return true;
	}

	ShaderProgram::ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src) {

		GLuint vertex_shader_id = 0;
		if (!createShader(vertex_shader_src, GL_VERTEX_SHADER, vertex_shader_id)) {
			LOG_CRITICAL("VERTEX SHADER");
			glDeleteShader(vertex_shader_id);
			return;
		}

		GLuint fragment_shader_id = 0;
		if (!createShader(fragment_shader_src, GL_FRAGMENT_SHADER, fragment_shader_id)) {
			LOG_WARN("FRAGMENT SHADER");
			glDeleteShader(vertex_shader_id);
			glDeleteShader(fragment_shader_id);
			return;
		}

		m_id = glCreateProgram();			   
		glAttachShader(m_id, vertex_shader_id);
		glAttachShader(m_id, fragment_shader_id);
		glLinkProgram(m_id);

		GLint success;
		glGetProgramiv(m_id, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			char info_log[1024];
			glGetShaderInfoLog(m_id, 1024, nullptr, info_log);
			LOG_CRITICAL("SHADER PROGRAM COMPILATION ERROR:\n{}", info_log);
			glDeleteProgram(m_id);
			m_id = 0;
			glDeleteShader(vertex_shader_id);
			glDeleteShader(fragment_shader_id);
			return;
		}
		m_isCompiled = true;
		glDetachShader(m_id, vertex_shader_id);
		glDetachShader(m_id, fragment_shader_id);
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
	}

	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(m_id);
	}

	void ShaderProgram::bind() const{
		glUseProgram(m_id);
	}
	void ShaderProgram::unbind() {
		glUseProgram(0);
	}

	void ShaderProgram::set_matrix4(const char* name, glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void ShaderProgram::set_int(const char* name, const int value) {
		glUniform1i(glGetUniformLocation(m_id, name), value);
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderprogram) noexcept{
		glDeleteProgram(m_id);
		m_id = shaderprogram.m_id;
		m_isCompiled = shaderprogram.m_isCompiled;

		shaderprogram.m_id = 0;
		shaderprogram.m_isCompiled = false;
		return *this;
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& shaderprogram) noexcept{
		m_id = shaderprogram.m_id;
		m_isCompiled = shaderprogram.m_isCompiled;

		shaderprogram.m_id = 0;
		shaderprogram.m_isCompiled = false;
	}
}