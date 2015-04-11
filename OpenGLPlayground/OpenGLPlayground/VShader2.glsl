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

const mat2 m2 = mat2(1.6,-1.2,1.2,1.6);

float terrainMed( vec2 p )
{
    p *= 0.0013;

    float s = 1.0;
	float t = 0.0;
	for( int i=0; i<9; i++ )
	{
        t += s*tri( p );
		s *= 0.5 + 0.1*t;
        p = 0.97*m2*p + (t-0.5)*0.2;
	}
            
    return t*55.0;
}

void main() {

  gl_Position = ProjMat * MVmatrix * vec4 (vertex_position, 1.0) + vec4(divisor * terrainMed(vertex_normal.xy/divisor), divisor * terrainMed(vertex_normal.xy/divisor), 0.0, 1.0) - vec4(divisor * 85, divisor * 85, 0.0, 1.0);
  vec4 vertPos4 = MVmatrix * vec4(vertex_position, 1.0) + vec4(divisor * terrainMed(vertex_normal.xy/divisor), divisor * terrainMed(vertex_normal.xy/divisor), 0.0 , 1.0);
  vertPos = vec3(vertPos4) / vertPos4.w;

  normal = transpose(inverse(MVmatrix)) * vec4(vertex_normal, 1.0);

  viewDirection = MVmatrix * viewDir;

  texCoords = texture_coordinates;
}