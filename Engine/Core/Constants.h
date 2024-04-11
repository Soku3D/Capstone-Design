#pragma once

namespace soku {
struct GlobalConstants {
    Matrix viewProj;
    Matrix view;
    Matrix proj;
    Vector3 eyePos;
    float lod;
};
struct MeshConstants {
    Matrix world;
    Matrix worldIT;
    float heightScale;
    Vector3 dummy;
};
struct SamplingPSConstants {
    float dx;
    float dy;
    float expose;
    float gamma;
    float bloomStrength;
    Vector3 dummy1;
};
} // namespace soku