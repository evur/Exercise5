attribute vec3 pos;
attribute vec2 tex;
varying vec2 texCoord;

void kore() {
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	texCoord = tex;
}
