#version 330 core
out vec4 fragColor;
in vec2 texCoords;
in vec3 NormalVec;
flat in int is_solid_color;
in vec3 color;
uniform sampler2D geometry_texture;

void main() {
  bool solid_color = bool(is_solid_color);
  if (solid_color) {
    fragColor = vec4(color, 1.0f);
  } else {
    fragColor = vec4(color, 1.0f);
  }
}
