#include "gpu_particles_utils.hlsli"

cbuffer cb0
{
    ComputeConstants constants;
};
cbuffer cb1
{
    ParameterData paramData;
};
cbuffer cb2
{
    EmitterData emitter;
};

RWStructuredBuffer<ParticleData> Particles;

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint particleID = emitter.ParticleHead + dtid.x;

    ParticleData particle;
    particle.FlagBits = 0;
    particle.Seed = 0;
    particle.LifeAge = 0.0f;
    particle.InheritColor = 0;
    particle.Color = 0;
    particle.Direction = 0;
    particle.Velocity = uint2(0, 0);
    particle.Transform = float4x3(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    Particles[particleID] = particle;
}
