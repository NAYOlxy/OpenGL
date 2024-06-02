#pragma once

#include <vector>
#include "Renderer.h"
#include <GL/glew.h>


template <typename T>
struct always_false {
	static constexpr bool value = false;
};

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout() :m_Stride(0) {

	}

	template<typename T>
	void Push(unsigned int count)
	{
		// 这个地方直接写false会直接报错，在模板定义时就触发断言了
		// 在编译时，首先就检查到静态断言失败了，所以这个模板连实例化的机会都没有就被断言了
		// 所以我们在模板函数或类中使用always_false来触发static_assert，确保它只在模板实例化时触发
		static_assert(always_false<T>::value);

	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT,count,GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT , count,GL_FALSE });
		m_Stride += count*VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE , count,GL_TRUE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};
