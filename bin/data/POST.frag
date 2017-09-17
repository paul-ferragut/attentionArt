#version 120

//#define USE_MIPMAP

// The Golden Angle is (3.-sqrt(5.0))*PI radians, which doesn't precompiled for some reason.
// The compiler is a dunce I tells-ya!!
#define GOLDEN_ANGLE 2.39996323

#define ITERATIONS 140

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect u_tex_unit0;
uniform float elapsedTime;
uniform float alpha;
uniform vec2 resolution;
uniform float val1;
uniform float val2;
uniform float val3;
uniform float val4;
uniform float val5;//range mouse
uniform float val6;
uniform float val7;
uniform float val8;
uniform float val9;
uniform float val10;

//uniform vec2 pos;

#define MAX_ITEMS 10
uniform vec2 posP[MAX_ITEMS];
uniform float radiusP[MAX_ITEMS];

mat2 rot = mat2(cos(GOLDEN_ANGLE), sin(GOLDEN_ANGLE), -sin(GOLDEN_ANGLE), cos(GOLDEN_ANGLE));

vec2 distortUV(vec2 uv, vec2 nUV)
{
    float intensity = 0.01;
    float scale = 0.01;
    float speed = 0.25;
    
    
    nUV.x += (elapsedTime)*speed;
    nUV.y += (elapsedTime)*speed;
    //vec2 noise= texture2D( iChannel0, nUV*scale).xy;

   vec2 noise= texture2DRect(u_tex_unit0, nUV *scale).xy;
    
    uv += (-1.0+noise*2.0) * intensity;
    
    return uv;
}

// change these values to 0.0 to turn off individual effects


// Noise generation functions borrowed from: 
// https://github.com/ashima/webgl-noise/blob/master/src/noise2D.glsl

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float staticV(vec2 uv) {
    float staticHeight = snoise(vec2(9.0,elapsedTime*1.2+3.0))*0.3+5.0;
    float staticAmount = snoise(vec2(1.0,elapsedTime*1.2-6.0))*0.1+0.3;
    float staticStrength = snoise(vec2(-9.75,elapsedTime*0.6-3.0))*2.0+2.0;
	return (1.0-step(snoise(vec2(5.0*pow(elapsedTime,2.0)+pow(uv.x*7.0,1.2),pow((mod(elapsedTime,100.0)+100.0)*uv.y*0.3+3.0,staticHeight))),staticAmount))*staticStrength;
}

vec3 Bokeh(sampler2DRect tex, vec2 uv, float radius, float amount)
{
  vec3 acc = vec3(0.0);
  vec3 div = vec3(0.0);
    vec2 pixel = 1.0 / resolution.xy;
    float r = 1.0;
    vec2 vangle = vec2(0.0,radius); // Start angle
    amount += radius*500.0;
    
  for (int j = 0; j < ITERATIONS; j++)
    {  
        r += 1. / r;
      vangle = rot * vangle;
        // (r-1.0) here is the equivalent to sqrt(0, 1, 2, 3...)
        #ifdef USE_MIPMAP
        vec3 col = texture2DRect(tex, uv + pixel * (r-1.) * vangle).xyz;//, radius*1.25
        #else
        vec3 col = texture2DRect(tex, uv + pixel * (r-1.) * vangle).xyz;
        #endif
        col = col * col * 1.5; // ...contrast it for better highlights - leave this out elsewhere.
    vec3 bokeh = pow(col, vec3(9.0)) * amount+.4;
    acc += col * bokeh;
    div += bokeh;
  }
  return acc / div;
}


void main( void )
{




float vertJerkOpt = 90.0;
float vertMovementOpt = 1.0*20.0;
float bottomStaticOpt = 0.5;
float scalinesOpt = 1.0;
float rgbOffsetOpt = 2.0*val2;
float horzFuzzOpt = 10.0*(val1*2.0);


float distortion=0.0;  



for(int i=0;i<10;i++){

vec2 nPosP=posP[i];
//nPosP.y-=1.0; 
//nPosP.y*=-1.0; 
//nPosP.y/=-1.0;
float range=radiusP[i]*(val3*10.0);
float rangeY=radiusP[i]*(resolution.x/resolution.y);
  vec2 normalisedPos=nPosP/resolution;//;pos/resolution;//

  distortion+=(val1*0.25)
  *(smoothstep(normalisedPos.x-range,normalisedPos.x,gl_FragCoord.x/resolution.x)*1.0-smoothstep(normalisedPos.x,normalisedPos.x+range,gl_FragCoord.x/resolution.x))

  *(smoothstep(normalisedPos.y,normalisedPos.y-rangeY,gl_FragCoord.y/resolution.y)*1.0-smoothstep(normalisedPos.y+rangeY,normalisedPos.y,gl_FragCoord.y/resolution.y));

};

distortion=clamp(-distortion,0.0,val1);

horzFuzzOpt = 10.0*distortion*2.0;
rgbOffsetOpt = 2.0*val2*distortion;;//
float vertzFuzzOpt = 10.0*(distortion*2.0);

	vec2 uv = gl_TexCoord[0].st/resolution.xy;//;//gl_FragCoord.xy/resolution.xy;//*resolution.xy;
	

	float jerkOffset = (1.0-step(snoise(vec2(elapsedTime*0.01,5.0)),0.8))*0.05;
	
	float fuzzOffset = snoise(vec2(elapsedTime*15.0,uv.y*80.0))*0.003;

	float moveOnY=45.0*(cos(elapsedTime)*uv.x);
	float moveOnX=0.004;
	float largeFuzzOffset = snoise(vec2(elapsedTime*1.0,uv.y*moveOnY))*moveOnX;
    
    float vertMovementOn = (1.0-step(snoise(vec2(elapsedTime*0.2,8.0)),0.4))*vertMovementOpt;
    float vertJerk = (1.0-step(snoise(vec2(elapsedTime*1.5,5.0)),0.6))*vertJerkOpt;
    float vertJerk2 = (1.0-step(snoise(vec2(elapsedTime*5.5,5.0)),0.2))*vertJerkOpt;
    float yOffset = abs(sin(elapsedTime)*4.0)*vertMovementOn+vertJerk*vertJerk2*0.3;
    float y = mod(uv.y*resolution.y+yOffset,1.0);
    
	
	float xOffset = (fuzzOffset + largeFuzzOffset) * horzFuzzOpt;
     yOffset = (fuzzOffset + largeFuzzOffset) * vertzFuzzOpt;
    float staticVal = 0.0;
   
    for (float y = -1.0; y <= 1.0; y += 1.0) {
        float maxDist = 5.0/200.0;
        float dist = y/200.0;
    	staticVal += staticV(vec2(uv.x*resolution.x,uv.y+dist*resolution.y))*(maxDist-abs(dist))*1.5;
    }
        
    staticVal *= bottomStaticOpt;
	
	vec2 newTexCoord;
	//newTexCoord.s = gl_TexCoord[0].s + (cos(elapsedTime + (gl_TexCoord[0].s*0.01)) * 10.0);
//	newTexCoord.t = gl_TexCoord[0].t + (sin(elapsedTime + (gl_TexCoord[0].t*0.01)) * 10.0);

	//vec2 texCoordRed 	= gl_TexCoord[0].r;
	//vec2 texCoordGreen 	= gl_TexCoord[0].g;
	//vec2 texCoordBlue 	= gl_TexCoord[0].b;

  float red = texture2DRect(u_tex_unit0, vec2(uv.x + xOffset -0.01*rgbOffsetOpt,uv.y+yOffset) *resolution.xy).r+staticVal;
  float green = texture2DRect(u_tex_unit0, vec2(uv.x + xOffset,uv.y+yOffset)*resolution.xy).g;//+staticVal //add some noise
  float blue = texture2DRect(u_tex_unit0, vec2(uv.x+ xOffset +0.01*rgbOffsetOpt,uv.y+yOffset)*resolution.xy).b+staticVal;   


  vec3 color = vec3(red,green,blue);
 // float scanline = sin(uv.y*resolution.y*500.0)*0.04*scalinesOpt;
  //scanline = 0.0;//sin

  //color -= scanline;
	
	gl_FragColor = vec4(color,1.0);

	gl_FragColor.a=1.0;




}