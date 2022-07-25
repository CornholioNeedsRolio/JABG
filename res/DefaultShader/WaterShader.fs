#version 330

out vec4 color_output;

in vec2 out_texcoord;
in vec4 out_color;
in vec3 out_normal;
in mat4 out_view;
in vec2 out_uv;

uniform sampler2D texture0;

uniform vec3 lightDirection;

uniform int doesHaveTexture;

vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}

void main()
{
	if(doesHaveTexture != 0)
		color_output = texture2D(texture0, out_texcoord);
	else
		color_output = out_color;

	mat4 inv = inverse(out_view);

	vec3 pos = vec3(inv[3]);
	vec3 dir = -vec3(inv[2]);

	float ambientStrength = noise(out_uv)*0.25+1;
    vec3 ambient = ambientStrength * vec3(1,1,1);
		
	color_output = color_output*vec4(ambient,1);
	color_output.w = 0.35;
	
}
