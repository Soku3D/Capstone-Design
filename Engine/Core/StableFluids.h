#pragma once

namespace soku {
class StableFluids {
  public:
    StableFluids() {}
    void Initialize();

  public:
    void Update(float &dt, Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context);
    void Sourcing(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void Projection(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void Advection(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void Render(float &dt);
  private:
};
} // namespace soku