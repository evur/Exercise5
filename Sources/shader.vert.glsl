#version 450

in vec3 pos;
in vec2 tex;
in vec3 nor;
out vec2 texCoord;
out vec3 normal;

void main() {
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	texCoord = tex;
	normal = nor;
}
