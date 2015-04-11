#version 420

uniform mat4 ProjMat;
uniform mat4 MVmatrix;
uniform vec4 viewDir;
uniform vec4 normMatrix;
uniform float divisor;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coordinates;

out vec3 vertPos;
out vec4 normal;
out vec4 viewDirection;
out vec2 texCoords;

float tri( in vec2 p )
{
    vec2 q = 2.0*abs(fract(p)-0.5);
    q = q*q*(3.0-2.0*q);
    return -1.0 + q.x + q.y;  
}


void main() {


  gl_Position = ProjMat * MVmatrix * vec4 (vertex_position, 1.0) + vec4(divisor * tri(vertex_normal.xy/divisor), divisor * tri(vertex_normal.xy/divisor), 0.0, 1.0);
  vec4 vertPos4 = MVmatrix * vec4(vertex_position, 1.0) + vec4(divisor * tri(vertex_normal.xy/divisor), divisor * tri(vertex_normal.xy/divisor), 0.0 , 1.0);
  vertPos = vec3(vertPos4) / vertPos4.w;

  normal = transpose(inverse(MVmatrix)) * vec4(vertex_normal, 1.0);

  viewDirection = MVmatrix * viewDir;

  texCoords = texture_coordinates;

}