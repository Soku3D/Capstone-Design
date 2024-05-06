#pragma once

namespace soku {
class StableFluids {
  public:
    StableFluids() {}
    void Initialize();

  public:
    void Update(float &dt, Microsoft::WRL::ComPtr<ID3D11DeviceContext> & context);
    void Sourcing();
    void Projection();
    void Advection();
    void Render(float &dt);
  private:
};
} // namespace soku