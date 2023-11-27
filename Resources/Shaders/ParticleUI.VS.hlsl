#include "ParticleUI.hlsli"

struct ParticleForGPU
{
    matrix world; // ワールド変換行列
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticleForGPU : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};


VSOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos = mul(gParticleForGPU[instanceID].world, input.position);
    output.uv = input.texcoord;
    output.color = gParticleForGPU[instanceID].color;
    return output;
}