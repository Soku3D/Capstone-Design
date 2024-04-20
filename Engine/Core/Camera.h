#pragma once

namespace soku {
class Camera {
  public:
    Camera();
    Matrix GetViewRow() const;
    Matrix GetCubeViewRow() const;
    Matrix GetProjRow() const;
    Vector3 GetEyePos() const;

    void UpdateMouse(int deltaX, int deltaY);
    void MoveForward(float dt);
    void MoveRight(float dt);
    void SetAspectRatio(float aspect);
    float m_speed = 1.0f;
    float m_rotationSpeed = 0.05f;

  public:
    Vector3 m_position = Vector3(0.0f, 0.f, -1.0f);
    Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);

    float m_pitch = 0.0f;
    float m_yaw = 0.0f;
    float delPitch = 0.0f;
    float delYaw = 0.0f;

    float m_projFovAngleY = 70.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
    float m_aspect = 16.0f / 9.0f;
    bool m_usePerspectiveProjection = true;
};
} // namespace soku