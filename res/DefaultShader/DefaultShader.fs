#version 330

out vec4 color_output;

in vec2 out_texcoord;
in vec4 out_color;
in vec3 out_normal;

in vec4 shadow_position;

 
uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 lightDirection;

uniform int doesHaveTexture;

float shadow()
{
	vec3 projCoords = shadow_position.xyz / shadow_position.w;
	projCoords = projCoords * 0.5 + 0.5;

	int outside = 0;
	if(projCoords.x < 0 || projCoords.y < 0 || projCoords.z < 0 || projCoords.x > 1 || projCoords.y > 1 || projCoords.z > 1)
		outside = 1;

	float op = 1;
	if(projCoords.z >= texture2D(texture1, projCoords.xy).r && outside == 0)
		op = 0;
	return op;
}

void main()
{
	if(doesHaveTexture != 0)
		color_output = texture2D(texture0, out_texcoord)*out_color;
	else
		color_output = out_color;

	/*float crs = min(1.0, max(min(shadow(), -dot(out_normal, lightDirection)), 0.0));
	//float crs = min(shadow(), -dot(out_normal, lightDirection));
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * vec3(1);

	color_output.xyz *= (ambient+crs);


	
	if(drawDepthMap == 0)
	{
		float crs = max(-dot(out_normal, out_dirLight), 0.0);
		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * vec3(1);
		
		if(doesHaveTexture != 0)
			coloroutput = texture2D(mytexture, texcoordsout)*vec4(colorout, 1);
		else
			coloroutput = vec4(colorout, 1);
			
		coloroutput.xyz *= (ambient+crs);
	}
	else
	{
		coloroutput = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1);
	}*/
}
