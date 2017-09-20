#version 120

uniform vec2 u_resolution;
uniform float p1;
uniform float p2;
uniform float p3;
uniform vec3 col1;
uniform vec3 col2;
uniform vec3 col3;
uniform vec3 col4;
uniform vec3 col5;

void main(){
    vec2 st = gl_FragCoord.xy/u_resolution;
    vec3 color = vec3(0.0);

    /*
     col1= vec4(0.5,0.7,0.1,1.0);
     col2= vec4(0.700,0.041,0.028,1.000);
     col3= vec4(0.260,0.635,0.635,1.000);
     col4= vec4(0.330,0.706,0.990,1.000);
     col5= vec4(0.251,0.175,0.990,1.000);
    
     p1=0.3;
     p2=0.7;
     p3=0.9;
     */
  
    color=col1;
    if(st.x> 0.0 && st.x < p1){
        color=mix(col1,col2,st.x/p1);
    }else if(st.x > p1 && st.x < p2){
        color=mix(col2,col3,smoothstep(p1,p2,st.x));
    }
    else if(st.x > p2 && st.x < p3){
        color=mix(col3,col4,smoothstep(p2,p3,st.x));
    }   
    else if(st.x > p3 ){
        color=mix(col4,col5,smoothstep(p3,1.0,st.x));
    }
    
    gl_FragColor = vec4(vec3(color),1.0);
}