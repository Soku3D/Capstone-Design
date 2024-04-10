#pragma once

namespace soku {
struct GlobalConstants {
    Matrix viewProj;
    Vector3 eyePos;
    float lod;
};
struct MeshConstants {
    Matrix world;
    Matrix worldIT;
};
struct SamplingPSConstants {
    float dx;
    float dy;
    float expose;
    float gamma;
};
} // namespace soku