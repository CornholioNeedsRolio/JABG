#version 330
layout(location=0) in vec3 position;
layout(location=1) in vec2 texCoords;
layout(location=2) in vec3 normal;

out vec2 out_texcoord;
out vec3 out_color;
out vec3 out_normal;


uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform vec3 color;

void main()
{
	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0);

	out_texcoord = texCoords;
	out_color = color;
	out_normal = normalize(normal*mat3(ModelMatrix));
}