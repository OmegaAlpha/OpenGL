#shader vertex
#version 330 core
layout (location = 0) in vec2 position;            
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;
void main(){
    texCoord = inTexCoord;
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}


#shader fragment
#version 330 core
in vec2 texCoord;
uniform vec4 iMouse;
uniform vec2 iResolution;
uniform float iTime;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;

//-----------------------------------------------------------------------

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

    // Output to screen
    fragColor = vec4(uv,0.0,1.0);
}
