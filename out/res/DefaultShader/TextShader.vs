#version 330
layout(location=0) in vec3 position;
layout(location=1) in vec2 texCoords;


uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;


out vec2 texcoordsout;

void main()
{
	gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0);
	texcoordsout = texCoords;
}