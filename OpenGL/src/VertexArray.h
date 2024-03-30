#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray(); 

	void Bind() const;
	void UnBind() const;


	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
private:

	unsigned int m_RendererID;
protected:
};

