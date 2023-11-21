#include "Bloom.hlsli"

#define NUM_TEXTURES 5

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
Texture2D<float4> tex2 : register(t2);
Texture2D<float4> tex3 : register(t3);
Texture2D<float4> tex4 : register(t4); 
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

struct Pram
{
    float intensity;
};
ConstantBuffer<Pram> param_ : register(b0);

PixelShaderOutPut main(VertexShaderOutPut input)
{
    PixelShaderOutPut output;
    float4 bloom = float4(0.0f, 0.0f, 0.0f, 0.0f);
    bloom += tex0.Sample(smp, input.texcoord)*param_.intensity;
    bloom += tex1.Sample(smp, input.texcoord)*param_.intensity;
    bloom += tex2.Sample(smp, input.texcoord)*param_.intensity;
    bloom += tex3.Sample(smp, input.texcoord)*param_.intensity;
    bloom += tex4.Sample(smp, input.texcoord)*param_.intensity;
    bloom /= NUM_TEXTURES;
    bloom.a = 1.0f;

    output.color = bloom;
    return output;
}