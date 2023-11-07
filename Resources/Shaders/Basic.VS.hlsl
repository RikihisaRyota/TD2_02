#include "Basic.hlsli"

struct WorldTransform
{
    matrix world; // ワールド変換行列
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);

struct ViewProjection
{
    matrix view; // ビュー変換行列
    matrix projection; // プロジェクション変換行列
    float3 cameraPos; // カメラのワールド座標
};

ConstantBuffer<ViewProjection> gViewProjection : register(b1);

struct CullingFlag
{
    int flag;
};

ConstantBuffer<CullingFlag> gCullingFlag : register(b2);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main( VertexShaderInput input ) {
    VertexShaderOutput output; // ピクセルシェーダーに渡す値
    if (!gCullingFlag.flag)
    {
        output.position = input.position + float4(normalize(input.normal) * 0.05f, 0.0f);
    }
    else
    {
        output.position = input.position;
    }
    output.position = mul(mul(output.position, gWorldTransform.world), mul(gViewProjection.view, gViewProjection.projection));
    output.normal = normalize(mul(input.normal, (float3x3)gWorldTransform.world));
    output.texcoord = input.texcoord;
    return output;
}