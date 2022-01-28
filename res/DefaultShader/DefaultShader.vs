#version 330
layout(location=0) in vec3 position;
layout(location=1) in vec2 texCoords;
layout(location=2) in vec3 normal;

out vec2 out_texcoord;
out vec4 out_color;
out vec3 out_normal;
out vec4 out_fragpos;
//out vec3 out_dirLight;

out vec4 shadow_position;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 DirLightMatrix;

uniform vec4 color;

void main()
{
	out_fragpos = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0);

	shadow_position = DirLightMatrix * ModelMatrix * vec4(position, 1.0);

	out_texcoord = texCoords;
	out_color = color;

	out_normal = normalize(normal*mat3(ModelMatrix));
	//out_dirLight = inverse(mat3(DirLightMatrix)) * vec3(0, 1, 1);
	gl_Position = out_fragpos;
}