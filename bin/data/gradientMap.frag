#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect gradient;
uniform float gradientWidth;  
varying vec2 texCoordVarying;

void main()
{
    // Get color value from
    vec3 src = texture2DRect(tex0, texCoordVarying).rgb;

	float gray = 0.2989 * src.r + 0.5870 * src.g + 0.1140 * src.b;
    // Get alpha value
    //float mask = texture2DRect(gradient, texCoordVarying).r;
vec3 map = texture2DRect(gradient, vec2(gray*gradientWidth, 0.0) ).rgb; //texCoordVarying
    // Set
   // gl_FragColor = vec4(src , mask);
   gl_FragColor = vec4( map.r,map.g,map.b, texture2DRect(tex0, texCoordVarying).a );
}





