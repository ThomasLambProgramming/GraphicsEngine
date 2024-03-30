#pragma once
#include "Renderer.h"


class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	//local storage for the texture its unsigned char because each unsigned char is 1 byte
	unsigned char* m_LocalBuffer; 
	//bits per pixel = bpp
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();

	//having a 0 there means that you can not supply a function and it will give a default value
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

