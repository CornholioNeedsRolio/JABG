#version 330

out vec4 color;
in vec2 texcoordsout;

uniform sampler2D texture0;

void main()
{
	float alpha = texture2D(texture0, texcoordsout).r;
	///if(alpha<=0.1)discard;
	color = vec4(1.0, 1.0, 1.0, 1.0)*alpha;
}
