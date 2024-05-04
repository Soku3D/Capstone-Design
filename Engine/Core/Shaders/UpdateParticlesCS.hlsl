struct Particle
{
    float3 position;
    float3 color;
    float3 velocity;
    float3 acceleration;
    float width;
    float time;
    float mass;
    float density;
    float pressure;
};
#define PI 3.141592
static const float3 g = float3(0, -9.8f, 0.f);
cbuffer CSConsts : register(b0)
{
    float dt;
    float k;
    float friction;
    float rho_0;
}
cbuffer CSConstant : register(b1)
{
    float j;
    float f;
}

    RWStructuredBuffer<Particle> Particles : register(u0);
float GetWeight(float q)
{
    
    float coeff = 3.0f / (2.0f * 3.141592f);

    if (q < 1.0f)
        return coeff * (2.0f / 3.0f - q * q + 0.5f * q * q * q);
    else if (q < 2.0f)
        return coeff * pow(2.0f - q, 3.0f) / 6.0f;
    else // q >= 2.0f
        return 0.0f;
}
float GetDelWeight(float q)
{
    
    float coeff = 3.0f / (2.0f * 3.141592f);

    if (q < 1.0f)
        return coeff * (-2.0f * q + 1.5f * q * q);
    else if (q < 2.0f)
        return coeff * -0.5f * (2.0f - q) * (2.0f - q);
    else // q >= 2.0f
        return 0.0f;
}
[numthreads(256, 1, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    Particle p_i = Particles[dtID.x];
    Particle p_j = Particles[j];
    float h = p_i.width;
    float dist = length(p_i.position - p_j.position);
  
    if (f == 0)
    {
        if (dist >= h)
            return;
        float w = GetWeight((2.f * dist) / h);
        p_i.density += w;
        Particles[dtID.x] = p_i;
    }
    if (f == 1)
    {
        if (dtID.x == j)
            return;
        if (dist < 1e-3f) 
            return;
        float3 x_i = p_i.position;
        float3 x_j = p_j.position;
        float3 x_ij = x_i - x_j;
        float3 v_i = p_i.velocity;
        float3 v_j = p_j.velocity;
        float3 v_ij = v_i - v_j;
        
        float rho_i = p_i.density;
        float rho_j = p_j.density;
        
        float pr_i = pow(rho_i , 7) - 1.f;
        float pr_j = pow(rho_j , 7) - 1.f;
        
        const float3 gradPressure =
                rho_i * 1.f *
                (pr_i / (rho_i * rho_i) + pr_j / (rho_j * rho_j)) *
                GetDelWeight(dist * 2.0f / h) * (x_i - x_j) / dist;

        const float3 laplacianVelocity =
                2.f * (1.f / rho_j) * (v_i - v_j) /
                 (dot(x_ij, x_ij) + (0.01f * h * h)) *
                GetDelWeight(dist * 2.0f / h) * dot(x_ij, x_ij / dist);

        const float3 pressureForce = -gradPressure / rho_i ;
        const float3 viscosityForce = 0.1f * laplacianVelocity;
        p_i.acceleration += pressureForce + viscosityForce;

        Particles[dtID.x] = p_i;
    }
    if (f == 2)
    {
        p_i.density = 0.f;
        p_i.acceleration = float3(0.f, 0.f, 0.f);
    }
    if (f == 3)
    {
        if (p_i.time < 0.f)
            return;
        p_i.velocity += (p_i.acceleration + g) * dt;
        p_i.position += (p_i.velocity) * dt;
        const float cor = 0.5f;
        if (p_i.velocity.y < 0.f && p_i.position.y < -0.8f)
        {
            p_i.position.y = -0.8f;
            p_i.velocity.y *= -cor;
        }
        if (p_i.velocity.x < 0.f && p_i.position.x < -0.8f)
        {
            p_i.position.x = -0.8f;
            p_i.velocity.x *= -cor;
        }
        if (p_i.velocity.x > 0.f && p_i.position.x > 0.8f)
        {
            p_i.position.x = 0.8f;
            p_i.velocity.x *= -cor;
        }
        Particles[dtID.x] = p_i;

    }
  
}