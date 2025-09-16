Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float4 options;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    // Âü°í: https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
    
    float3 a = g_texture0.Sample(g_sampler, float2(input.texcoord.x - dx, input.texcoord.y + dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(input.texcoord.x     , input.texcoord.y + dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(input.texcoord.x + dx, input.texcoord.y + dy)).rgb;
    
    float3 d = g_texture0.Sample(g_sampler, float2(input.texcoord.x - dx, input.texcoord.y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(input.texcoord.x     , input.texcoord.y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(input.texcoord.x + dx, input.texcoord.y)).rgb;
    
    float3 g = g_texture0.Sample(g_sampler, float2(input.texcoord.x - dx, input.texcoord.y - dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(input.texcoord.x     , input.texcoord.y - dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(input.texcoord.x + dx, input.texcoord.y - dy)).rgb;
    
    float3 upSample = e * 4.0;
    upSample += (b + d + f + h) * 2.0;
    upSample += (a + c + g + i);
    upSample *= 1.0 / 16.0;
  
    return float4(upSample, 1.0);
}
