#version 330

out vec4 color_output;

in vec2 out_texcoord;
in vec3 out_color;
in vec3 out_normal;

uniform sampler2D texture0;

uniform int doesHaveTexture;

void main()
{
    float depthValue = texture(texture0, out_texcoord).r;
    color_output = vec4(vec3(depthValue), 1.0);
}
