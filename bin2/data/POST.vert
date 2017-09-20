/*
attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;
attribute vec2 texcoord;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;


//varying vec3 position, normal;
varying float randomOffset;
varying vec4 colorVarying;
varying vec2 texCoordVarying;


void main()
{
	vec4 pos = projectionMatrix * modelViewMatrix * position;
	gl_Position = pos;
	
	colorVarying = color;
	texCoordVarying = texcoord;
}
*/
/*

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {

	gl_FrontColor = gl_Color;
	//gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	position = gl_Vertex.xyz;
	normal = gl_Normal.xyz;
	randomOffset = rand(gl_Vertex.xy + gl_Vertex.yz);

	colorVarying = color;
	texCoordVarying = texcoord;



}
*/
#version 120
void main()
{
   gl_TexCoord[0] = gl_MultiTexCoord0;
  // texCoord = vec2(gl_MultiTexCoord0); 
    gl_Position    = ftransform();
    	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}