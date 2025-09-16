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
    
    float3 a = g_texture0.Sample(g_sampler, float2(input.texcoord.x - 2.0 * dx, input.texcoord.y + 2.0 * dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(input.texcoord.x           , input.texcoord.y + 2.0 * dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(input.texcoord.x + 2.0 * dx, input.texcoord.y + 2.0 * dy)).rgb;
    
    float3 d = g_texture0.Sample(g_sampler, float2(input.texcoord.x - 2.0 * dx, input.texcoord.y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(input.texcoord.x           , input.texcoord.y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(input.texcoord.x + 2.0 * dx, input.texcoord.y)).rgb;
    
    float3 g = g_texture0.Sample(g_sampler, float2(input.texcoord.x - 2.0 * dx, input.texcoord.y - 2.0 * dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(input.texcoord.x           , input.texcoord.y - 2.0 * dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(input.texcoord.x + 2.0 * dx, input.texcoord.y - 2.0 * dy)).rgb;
    
    float3 j = g_texture0.Sample(g_sampler, float2(input.texcoord.x - dx, input.texcoord.y + dy)).rgb;
    float3 k = g_texture0.Sample(g_sampler, float2(input.texcoord.x + dx, input.texcoord.y + dy)).rgb;
    float3 l = g_texture0.Sample(g_sampler, float2(input.texcoord.x - dx, input.texcoord.y - dy)).rgb;
    float3 m = g_texture0.Sample(g_sampler, float2(input.texcoord.x - dx, input.texcoord.y - dy)).rgb;
    
    float3 downSample = e * 0.125;
    downSample += (a + c + g + i) * 0.03125;
    downSample += (b + d + f + h) * 0.0625;
    downSample += (j + k + l + m) * 0.125;
  
    return float4(downSample, 1.0);
}