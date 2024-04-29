#pragma once
#include "pch.h"
#include "BaseApp.h"
#include "Mesh.h"
#include "Model.h"
#define MAX_LIGHTS 3

namespace soku {
class RenderApp : public BaseApp {
  public:
    RenderApp(const int width, const int height);
    virtual bool Initialize() override;
    
  private:
    virtual void Render(float deltaTime) override;
    void RenderSpinParticles();
    virtual void Update(float deltaTime) override;
    virtual void UpdateGUI(float deltaTime) override;

  private:
    std::vector<std::shared_ptr<Model>> models;
    std::shared_ptr<Model> skybox;
    std::shared_ptr<Model> mirror;
    std::shared_ptr<Model> dotModel;
    std::shared_ptr<Model> screen;
    //GlobalCosntants
    float textureLOD;
    //MeshConstants
    float heightScale;
    
    SamplingPSConstants SPSconstant;
    Matrix mirrorMat;
    int useEnv = 1;
};
} // namespace soku