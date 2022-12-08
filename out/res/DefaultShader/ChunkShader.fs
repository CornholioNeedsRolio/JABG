#version 330

out vec4 color_output;

in vec2 out_texcoord;
in vec4 out_color;
in vec3 out_normal;
in mat4 out_view;
 
uniform sampler2D texture0;

uniform vec3 lightDirection;

uniform int doesHaveTexture;

void main()
{
	if(doesHaveTexture != 0)
	{
		mat4 inv = inverse(out_view);

		vec3 pos = vec3(inv[3]);
		vec3 dir = -vec3(inv[2]);

		float ambientStrength = min(dot(-dir,out_normal)*0.2 + 0.8, 1);
    	vec3 ambient = ambientStrength * vec3(1,1,1);

		color_output = texture2D(texture0, out_texcoord)*out_color*vec4(ambient,1);
	}
	else
		color_output = out_color;
	if(color_output.w <=0.1) discard;
	
}
