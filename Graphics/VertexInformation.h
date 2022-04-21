#pragma once

#include <string>

#include <assimp/scene.h> 

struct sVertex_XYZW_RGBA_N_UV_T_B
{
    float x, y, z, w;   // Same as vec4 vPosition
    float r, g, b, a;   // Same as vec4 vColour
    float nx, ny, nz, nw;   // in vec4 vNormal;	Vertex normal X,Y,Z (W ignored)
    float u0, v0, u1, v1;   //in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
    float tx, ty, tz, tw;   //in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
    float bx, by, bz, bw;   //in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)
};

struct sVertex
{
    float x, y, z; 
    float nx, ny, nz;
    float u0, v0;
};

struct sColoredVertex
{
    float x, y, z;
    float nx, ny, nz;
    float r, g, b, a;
};

struct sTriangle
{
    unsigned int vertIndex[3];
};
