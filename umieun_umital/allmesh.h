#pragma once
#include "headers.h"
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

   
    int m_BoneIDs[4];
    float m_Weights[4];
};


struct MeshFileHeader {
    int numMeshes;     
    bool hasAnimation;  
};