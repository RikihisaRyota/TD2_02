#include "ParticleUI.hlsli"

Texture2D<float4> tex : register(t1); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * input.color;
}