#include "IndexBuffer.hpp"
#include "SimpleEngineCore/Log.hpp"
#include "glad/glad.h"


namespace SimpleEngine {

	constexpr GLenum usage_to_GLenum(const VertexBuffer::EUsage usage);

	IndexBuffer::IndexBuffer(const void* data, const size_t count, const VertexBuffer::EUsage usage)
		: m_count(count)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, usage_to_GLenum(usage));
	}

	SimpleEngine::IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_id);
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& indexBuffer) noexcept {
		m_count = indexBuffer.m_count;
		indexBuffer.m_count = 0;
		m_id = indexBuffer.m_id;
		indexBuffer.m_id = 0;
		return *this;
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& indexBuffer) noexcept
		: m_count(indexBuffer.m_count),
		  m_id(indexBuffer.m_id)
	{
		indexBuffer.m_count = 0;
		indexBuffer.m_id = 0;
	}

	void IndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void IndexBuffer::unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}