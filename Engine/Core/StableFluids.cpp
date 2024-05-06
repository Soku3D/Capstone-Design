#include "pch.h"
#include "StableFluids.h"

namespace soku {
void StableFluids::Initialize() {}
void StableFluids::Update(
    float &dt, Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    Sourcing(context);
    Projection(context);
    Advection(context);
}
void StableFluids::Sourcing(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {}
void StableFluids::Projection(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {}
void StableFluids::Advection(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {}
void StableFluids::Render(float &dt) {}
} // namespace soku
