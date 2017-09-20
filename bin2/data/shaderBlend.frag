#version 120

uniform sampler2DRect tex0;

uniform float blurAmnt;

varying vec2 texCoordVarying;

void main()
{
    vec4 color;
	
    color = texture2DRect(tex0, texCoordVarying);
    float alpha=((color.r+color.g+color.b)/3.0);
    gl_FragColor = vec4(color.r,color.g,color.b,alpha*0.5);
}