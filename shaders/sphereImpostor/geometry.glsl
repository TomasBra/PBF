#version 430

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform vec3 CameraPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float sphereRadius;

mat4 gVp;
flat out vec3 toCamera; //world space
out vec2 mapping; //(-R,-R) to (R,R)
flat out vec3 center; //world space
flat out vec4 graphics;
flat out vec3 up;
flat out vec3 right;

in DATA{
 vec4 graphics;
}data_in[];

float correction = 1.5f;

void main()
{

    vec3 Pos = gl_in[0].gl_Position.xyz;
    center = vec3(vec4(Pos,1));
    graphics = data_in[0].graphics;
    //Pos -> Camera
    toCamera = normalize(CameraPosition - Pos);
    up = vec3(0.0, 1.0, 0.0);
    right = normalize(cross(toCamera, up));

    up = normalize(cross(right, toCamera));

    mat4 gVP = Projection * View;

    vec3 PointPosition = Pos - correction * sphereRadius * right - correction * sphereRadius * up;
    mapping = correction * sphereRadius * vec2(-1,-1);
    gl_Position = gVP * vec4(PointPosition, 1.0);
    EmitVertex();

    PointPosition = Pos - correction * sphereRadius * right + correction * sphereRadius * up;
    mapping = correction * sphereRadius * vec2(-1,1);
    gl_Position = gVP * vec4(PointPosition, 1.0);
    EmitVertex();

    PointPosition = Pos + correction * sphereRadius * right - correction * sphereRadius * up;
    mapping =  correction * sphereRadius * vec2(1,-1);
    gl_Position = gVP * vec4(PointPosition, 1.0);
    EmitVertex();

    PointPosition = Pos + correction * sphereRadius * right + correction * sphereRadius * up;
    mapping = correction * sphereRadius * vec2(1,1);
    gl_Position = gVP * vec4(PointPosition, 1.0);
    EmitVertex();

    EndPrimitive();
}