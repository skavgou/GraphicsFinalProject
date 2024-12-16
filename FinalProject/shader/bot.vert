#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 joints;
layout(location = 4) in vec4 weights;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;

uniform mat4 MVP;
uniform mat4 JointMat[256];

void main() {
    // Transform vertex
    mat4 skinMatrix =
     weights.x * JointMat[int(joints.x)] +
     weights.y * JointMat[int(joints.y)] +
     weights.z * JointMat[int(joints.z)] +
     weights.w * JointMat[int(joints.w)];

     vec3 blendedNormal =
             weights.x * mat3(JointMat[int(joints.x)]) * vertexNormal
             + weights.y * mat3(JointMat[int(joints.y)]) * vertexNormal
             + weights.z * mat3(JointMat[int(joints.z)]) * vertexNormal
             + weights.w * mat3(JointMat[int(joints.w)]) * vertexNormal;


    gl_Position =  MVP * (skinMatrix * vec4(vertexPosition, 1.0));

    // World-space geometry
    worldPosition = vertexPosition;
    worldNormal = normalize(blendedNormal);
}
