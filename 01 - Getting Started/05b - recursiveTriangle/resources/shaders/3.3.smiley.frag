#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

#define S(a, b, t) smoothstep(a, b, t)
#define sat(x) clamp(x, 0.0, 1.0)

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform vec4 u_clearColor;
uniform float u_mixPct;
uniform sampler2D u_texture1;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

// Art of Code ShaderToy Tutorials
// https://www.youtube.com/playlist?list=PLGmrMu-IwbguU_nY2egTFmlg691DN7uE5

// normalize t to be a value between 0.0 and 1.0
// if t==a return 0.0
// if a < t < b return value such that 0.0 < t > 1.0
// if t==b return 1.0
float remap01(float a, float b, float t) {
    return sat((t-a) / (b-a));
}

// remap {a..b} to {c..d}
// if t==a return c
// if a < t < b return value such that c < t > d
// if t==b return d
float remap(float a, float b, float c, float d, float t) {
    return sat((t-a)/(b-a)) * (d-c) + c;
}

// similar to react01 but in 2 dimensions
// normalize uv to be 0.0 to 1.0 (in x and y) within rect
vec2 within(vec2 uv, vec4 rect) {
    return (uv-rect.xy)/(rect.zw-rect.xy);

}

vec4 brow(vec2 uv, float smile) {
    float offs = mix(.2, 0., smile);
    uv.y += offs;

    float y = uv.y;

    // skew y as a function of x
    uv.y += uv.x*mix(.5, .8, smile)-mix(.1,.3,smile);

    // adjust position along x (move brows farther apart)
    uv.x -= mix(.0, .1, smile);

    // move origin to center 
    uv -= .5;
    
    vec4 col = vec4(0.);
    
    float blur = .1;
    
    float d1 = length(uv);
    float s1 = S(.45, .45-blur, d1);
    float d2 = length(uv-vec2(.1, -.2)*.7);
    float s2 = S(.5, .5-blur, d2);
    
    float browMask = sat(s1-s2);
    
    float colMask = remap01(.7, .8, y)*.75;
    colMask *= S(.6, .9, browMask);
    colMask *= smile;
    vec4 browCol = mix(vec4(.4, .2, .2, 1.), vec4(1., .75, .5, 1.), colMask); 
   
    // shadow
    uv.y += .15-offs*.5;
    blur += mix(.0, .1, smile);
    d1 = length(uv);
    s1 = S(.45, .45-blur, d1);
    d2 = length(uv-vec2(.1,-.2)*.7);
    s2 = S(.5, .5-blur, d2);
    float shadowMask = sat(s1-s2);
    
    col = mix(col, vec4(0.,0,0.,1.), S(.0,1.,shadowMask)*.5);
    col = mix(col, browCol, S(.2, .4, browMask));
    
    return col;
}

vec4 eye(vec2 uv, float side, vec2 m, float smile) {
    uv -= .5; // move origin to center
    uv.x *= side;

    float d = length(uv);

    // exterior blue fade
    vec4 irisCol = vec4(.3,.5,1.,1.);
    vec4 col = mix(vec4(1.), irisCol, S(.1,.7,d)*.5); 

    // mask out everything outside of the eye
    col.a = S(.5,.48,d);

    // edge shadow in lower corner
    col.rgb *= 1.0 - S(.45,.5,d)*.5*sat(-uv.y-uv.x*side);

    d = length(uv-m*.5);

    // black iris outline
    col.rgb = mix(col.rgb, vec3(0.), S(.3 ,.28,d));

    // interior blue fade in center
    irisCol.rgb *= 1.0 + S(.3,.05,d);
    float irisMask = S(.28,.25,d);
    col.rgb = mix(col.rgb, irisCol.rgb, irisMask);

    d = length(uv-m*.6);

    // black pupils
    float pupilSize = mix(.4, .16, smile);
    float pupilMask = S(pupilSize, pupilSize*.85, d);
    pupilMask *= irisMask; // mask pupil with irisMask
    col.rgb = mix(col.rgb, vec3(0.), pupilMask);

    // two small highlights
    float t = u_time*3.;
    vec2 offs = vec2(sin(t+uv.y*25.), sin(t+uv.x*25.));
    offs *= .01*(1.-smile);

    uv += offs;

    float hightlight = S(.1, .09, length(uv-vec2(-.15,.15)));
    hightlight += S(.07, .05, length(uv+vec2(-.08,.08)));
    col.rgb = mix(col.rgb, vec3(1.0), hightlight);

    return col;
}

vec4 mouth(vec2 uv, float smile) {
    uv -= .5;
    vec4 col = vec4(.5, .18, .05, 1.);

    // domain distortion to make mouth shape
    uv.y *= 1.5;
    //uv.y -= uv.x*uv.x*2.0*(1.-smile);
    uv.y -= uv.x*uv.x*2.0*smile;
    uv.x *= mix(2.,1.,smile);

    // mouth shape (distorted circle)
    float d = length(uv);
    col.a = S(.5, .48, d);

    // teeth
    vec2 tUv = uv;
    tUv.y += (abs(uv.x)*.5+.1)*(1.-smile);
    float td = length(tUv-vec2(0.,.6));
    vec3 toothCol = vec3(1.)*S(.6,.35,d); // drop shadow on teeth
    col.rgb = mix(col.rgb, toothCol, S(.4,.37,td));

    // tongue
    td = length(uv+vec2(0.,.5));
    col.rgb = mix(col.rgb, vec3(1.,.5,.5), S(.5,.2,td));
    return col;
}

vec4 head(vec2 uv) {
    vec4 col = vec4(0.9,0.65,0.1,1.0);

    // mask circle for head
    float d = length(uv);
    col.a = S(0.5,0.49,d);

    float edgeShade = remap01(0.35,0.5,d);
    edgeShade *= edgeShade;
    col.rgb *= 1.0 - edgeShade*.3;

    // dark border
    col.rgb = mix(col.rgb, vec3(0.6,0.3,0.1), S(0.47,0.48,d));

    float highlight = S(0.41,0.405,d);
    highlight *= remap(0.41,-0.1,0.75,0.0,uv.y); // main fade
    highlight *= S(.18,.19,length(uv-vec2(.21,.08))); // stamped out eye socket
    col.rgb = mix(col.rgb, vec3(1.0), highlight);


    d = length(uv-vec2(.25,-.2));
    float cheek = S(.2,.01,d)*.4;
    cheek *= S(.18,.16,d);
    col.rgb = mix(col.rgb, vec3(1.,.1,.1), cheek);

    return col;
}

vec4 smiley(vec2 uv, vec2 m, float smile) {
    vec4 col = vec4(0.0);

    // side = -1 if uv.x < 0, +1 if uv.x > 0
    float side = sign(uv.x); 

    // mirror along uv.x
    uv.x = abs(uv.x);

    vec4 head  = head(uv);
    vec4 eye = eye(within(uv, vec4(.03,-.1,.37,.25)), side, m, smile);
    vec4 mouth = mouth(within(uv, vec4(-.3,-.4,.3,-.1)), smile);
    vec4 brow = brow(within(uv, vec4(.03,.2,.4,.45)), smile);

    col = mix(col, head, head.a);
    col = mix(col, eye, eye.a);
    col = mix(col, mouth, mouth.a);
    col = mix(col, brow, brow.a);
    return col;
}

void main(){
    vec2 uv = TexCoord.xy;// /u_resolution; // normalize coords 0.0 <> 1.0
    uv -= 0.5; // place origin at center coords -0.5 <> 0.5
    //uv.x *= u_resolution.x/u_resolution.y; // maintain 1:1 aspect ratio

    vec2 m = vec2(clamp(u_mouse.x, -.5f, .5f), clamp(u_mouse.y, -.5f, .5f)); ///u_resolution.xy; // normalized mouse pos coords 0.0 <> 1.0
    //m -= .5;

    // head distortion with mouse
    float d = dot(uv, uv);
    uv -= m*sat(.23-d);

    vec4 container = texture(u_texture1, TexCoord);
    FragColor = mix(u_clearColor, container, u_mixPct);
    
    float smile = cos(u_time)*.5+.5;
    vec4 smiley = smiley(uv, m, smile);
    FragColor = mix(FragColor, smiley, smiley.a);
}
