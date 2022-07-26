#pragma once
//#include <string>
#include "glm/mat4x4.hpp"

namespace SimpleEngine { 
	/*std::string downloadShaderSrc(const char* path);
	*/

	class ShaderProgram {
	public:
		ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src);
		ShaderProgram(ShaderProgram&&) noexcept;
		ShaderProgram& operator=(ShaderProgram&&) noexcept;
		~ShaderProgram();

		ShaderProgram() = delete;
		ShaderProgram(const ShaderProgram&&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void bind() const;
		static void unbind();
		bool isCompiled() const { return m_isCompiled; }
		void setMatrix4(const char* name, glm::mat4& matrix);

	private:
		bool m_isCompiled = false;
		unsigned int m_id = 0;
	};
}