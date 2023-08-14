#version 330 core
out vec4 fragColor;
in vec2 texCoords;

uniform sampler2D m_texture;

void main() 
{
	fragColor = texture(m_texture, texCoords);
}
