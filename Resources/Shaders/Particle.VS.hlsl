#include "Particle.hlsli"

struct ParticleForGPU
{
    matrix world; // ワールド変換行列
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticleForGPU : register(t0);

struct ViewProjection
{
    matrix view; // ビュー変換行列
    matrix projection; // プロジェクション変換行列
    float3 cameraPos; // カメラのワールド座標
};

ConstantBuffer<ViewProjection> gViewProjection : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input,uint instanceID:SV_InstanceID)
{
    VertexShaderOutput output; // ピクセルシェーダーに渡す値
    output.position = input.position;
    output.position = mul(mul(output.position, gParticleForGPU[instanceID].world), mul(gViewProjection.view, gViewProjection.projection));
    output.normal = normalize(mul(input.normal, (float3x3) gParticleForGPU[instanceID].world));
    output.texcoord = input.texcoord;
    output.color = gParticleForGPU[instanceID].color;
    return output;
}