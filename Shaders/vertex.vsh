struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexShaderInput
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};

void main(in VertexShaderInput VSIn,
    out VertexShaderOutput VSOut)
{
    VSOut.Position = mul(ModelViewProjectionCB.MVP,float4(VSIn.Position,1.0f));
    VSOut.Color = float4(VSIn.Color,1.0f);
}