#pragma once

namespace soku {
struct GlobalConstants {
    Matrix viewProj;
    Vector3 eyePos;
    float dummy1;
};
struct MeshConstants {
    Matrix world;
    Matrix worldIT;
};
} // namespace soku